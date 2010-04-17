#pragma once
#include "../common.hpp"
#include "../lexer/lexeme.hpp"
#include "node.hpp"

namespace Legion
{
	class String;
	class Type;

	struct ExpressionNode;
	
	struct TypeModifierNode:
		public ListNode
	{
	};

	typedef List<TypeModifierNode> TypeModifierList;

	struct TypeNode:
		public Node
	{
		TypeNode() : name(0) {}

		String *name;
		Range *range;

		TypeModifierList modifiers;
		
		NodeType node_type()
		{
			return Node::TYPE_NODE;
		}
		
		Type *validate(ValidationArgs &args);
	};
	
	struct TypePointerNode:
		public TypeModifierNode
	{
		NodeType node_type()
		{
			return Node::TYPE_POINTER_NODE;
		}
	};
	
	struct TypeArrayNode:
		public TypeModifierNode
	{
		ExpressionNode *size;

		NodeType node_type()
		{
			return Node::TYPE_ARRAY_NODE;
		}
	};
	
	class ArrayType;
	class PointerType;
	
	class Type
	{
		private:
			PointerType *indirect;
			List<ArrayType> arrays;

		protected:
			virtual bool is_exact(Type *other)
			{
				return this == other;
			}

		public:
			enum Kind
			{
				NATIVE_TYPE,
				TYPEDEF_TYPE,
				COMPOSITE_TYPE,
				FUNCTION_TYPE,
				ARRAY_TYPE,
				POINTER_TYPE
			};

			enum Compatibility
			{
				EXACT,
				TYPEDEF,
				STRONG,
				WEAK
			};

			Kind kind;

			Type(Kind kind) : indirect(0), kind(kind)
			{
			}
			
			Type *get_indirect(ValidationArgs &args);
			Type *get_array(ValidationArgs &args, size_t size);
			void compatible(ValidationArgs &args, ExpressionNode *node);
			void compatible(ValidationArgs &args, Type *type, ExpressionNode *node);

			virtual std::string string(bool show_typedef = true) = 0;

			static Type *resolve(Type *type);

			virtual bool compatible(ValidationArgs &args, Type *other, Compatibility compatibility)
			{
				if(compatibility == EXACT)
					return this == other;
				else
					return this == other || this == resolve(other);
			}

			virtual Type *compatible_unary_op(ValidationArgs &args, Lexeme::Type lexeme, bool &compatible) 
			{
				return 0;
			}

			virtual Type *compatible_binary_op(ValidationArgs &args, Lexeme::Type lexeme, bool &compatible) 
			{
				return 0;
			}
	};

	struct ImplicitConversion
	{
		Type *type;
		bool weak;
		ImplicitConversion *next;
	};

	class ImplicitConversionFunctions:
		public HashTableFunctions<Type *, ImplicitConversion *>
	{
		public:
			static bool compare_key_value(Type *key, ImplicitConversion *value)
			{
				return value->type == key;
			}

			static Type *get_key(ImplicitConversion *value)
			{
				return value->type;
			}

			static ImplicitConversion *get_value_next(ImplicitConversion *value)
			{
				return value->next;
			}

			static void set_value_next(ImplicitConversion *value, ImplicitConversion *next)
			{
				value->next = next;
			}
	};

	struct Operator
	{
		Lexeme::Type lexeme;
		Type *returns;
		Operator *next;
	};

	class OperatorFunctions:
		public HashTableFunctions<Lexeme::Type, Operator *>
	{
		public:
			static bool compare_key_value(Lexeme::Type key, Operator *value)
			{
				return value->lexeme == key;
			}

			static Lexeme::Type get_key(Operator *value)
			{
				return value->lexeme;
			}

			static Operator *get_value_next(Operator *value)
			{
				return value->next;
			}

			static void set_value_next(Operator *value, Operator *next)
			{
				value->next = next;
			}
	};

	class Operators:
		public HashTable<Lexeme::Type, Operator *, OperatorFunctions>
	{
		public:
			Operators(MemoryPool &memory_pool) : HashTable<Lexeme::Type, Operator *, OperatorFunctions>(memory_pool, 1) {}

			void add(Lexeme::Type lexeme, Type *returns)
			{
				Operator *op = new (memory_pool) Operator;

				op->lexeme = lexeme;
				op->returns = returns;

				set(lexeme, op);
			}
	};

	class NativeType:
		public Type
	{
		public:
			String *name;
			NativeType *base;

			HashTable<Type *, ImplicitConversion *, ImplicitConversionFunctions> implicit_conversions;

			Operators unary_operators;
			Operators binary_operators;

			void add_conversion(Type *type, bool weak = false)
			{
				ImplicitConversion *conversion = new (implicit_conversions.memory_pool) ImplicitConversion;

				conversion->type = type;
				conversion->weak = weak;

				implicit_conversions.set(type, conversion);
			}

			std::string string(bool show_typedef)
			{
				return name->string();
			}

			Type *compatible_unary_op(ValidationArgs &args, Lexeme::Type lexeme, bool &compatible) 
			{
				Operator *op = unary_operators.get(lexeme);

				if(op)
				{
					compatible = true;

					return op->returns;
				}
				else
					return 0;
			}

			Type *compatible_binary_op(ValidationArgs &args, Lexeme::Type lexeme, bool &compatible) 
			{
				Operator *op = binary_operators.get(lexeme);

				if(op)
				{
					compatible = true;

					return op->returns;
				}
				else
					return 0;
			}

			bool compatible(ValidationArgs &args, Type *other, Compatibility compatibility)
			{
				if(compatibility == EXACT)
					return this == other;

				Type *type = resolve(other);

				if(this == type)
					return true;

				if(compatibility >= STRONG)
				{
					ImplicitConversion *conversion = implicit_conversions.get(type);

					if(conversion && (compatibility >= WEAK || !conversion->weak))
						return true;
				}

				if(base)
					return base->compatible(args, other, compatibility);
				else
					return false;
			}

			NativeType(MemoryPool &memory_pool, NativeType *base) : Type(Type::NATIVE_TYPE), base(base), implicit_conversions(memory_pool, 1), unary_operators(memory_pool), binary_operators(memory_pool) {}
	};

	struct TypeNativeNode:
		public Node
	{
		NativeType *type;

		NodeType node_type()
		{
			return Node::TYPE_NATIVE_NODE;
		}

		Type *get_type(ValidationArgs &args)
		{
			return type;
		}
	};

	class CompositeType:
		public Type
	{
		public:
			class Field
			{
				public:
					Type *type;
					String *name;

					Field *next;
			};

			CompositeType() : Type(Type::COMPOSITE_TYPE) {}

			String *name;
			List<Field> fields;

			Type *get_member(ValidationArgs &args, String *name, Range &range);
			
			std::string string(bool show_typedef = true)
			{
				return name->string();
			}
	};

	class FunctionType:
		public Type
	{
		public:
			class Parameter
			{
				public:
					Type *type;
					Parameter *next;
			};

			FunctionType() : Type(Type::FUNCTION_TYPE) {}

			Type *returns;
			List<Parameter> params;
			
			std::string string(bool show_typedef)
			{
				std::string result = returns->string() + "(";

				for(List<Parameter>::Iterator i = params.begin(); i; i++)
				{
					result += i().type->string();

					if(i().next)
						result += ", ";
				}

				return result + ")";
			}

		protected:
			bool compatible(ValidationArgs &args, Type *other, Compatibility compatibility)
			{
				Type *type = compatibility >= TYPEDEF ? resolve(other) : other;

				if(other->kind != Type::FUNCTION_TYPE)
					return false;

				if(this == other)
					return true;

				FunctionType *func = (FunctionType *)type;

				if(func->params.size != params.size)
					return false;

				if(!func->returns->compatible(args, returns, compatibility))
					return false;

				List<Parameter>::Iterator j = params.begin();

				for(List<Parameter>::Iterator i = func->params.begin(); i; i++, j++)
					if(!i().type->compatible(args, j().type, compatibility))
						return false;

				return true;
			}
	};

	class TypedefType:
		public Type
	{
		public:
			TypedefType(Type *base) : Type(Type::TYPEDEF_TYPE), base(base) {}

			String *name;
			Type *base;

			std::string string(bool show_typedef)
			{
				if(!base)
					return name->string();

				if(show_typedef)
					return name->string() + " (" + base->string(false) + ")";
				else
					return base->string(false);
			}

			bool compatible(ValidationArgs &args, Type *other, Compatibility compatibility)
			{
				if(!base)
					return true;

				if(compatibility == EXACT)
					return this == other;
				else
					return this == other || base->compatible(args, other, compatibility);
			}
	};

	class ArrayType:
		public Type
	{
		public:
			ArrayType(Type *base, size_t size) : Type(Type::ARRAY_TYPE), base(base), size(size) {}

			Type *base;
			size_t size;
			ArrayType *next;

			std::string string(bool show_typedef)
			{
				std::stringstream out;

				out << base->string(show_typedef) << "[" << size << "]";

				return out.str();
			}
	};

	class PointerType:
		public Type
	{
		public:
			PointerType(Type *base) : Type(Type::POINTER_TYPE), base(base) {}

			Type *base;

			std::string string(bool show_typedef)
			{
				return base->string(show_typedef) + "*";
			}

			Type *compatible_unary_op(ValidationArgs &args, Lexeme::Type lexeme, bool &compatible) 
			{
				if(lexeme == Lexeme::MUL)
				{
					compatible = true;

					return base;
				}
				else
					return 0;
			}

			bool compatible(ValidationArgs &args, Type *other, Compatibility compatibility);
	};

	class Compiler;

	class Types
	{
		private:
			Compiler &compiler;

			void declare(TypeNativeNode *parent, const char *name, TypeNativeNode &type, bool declare);

		public:
			TypeNativeNode type_null;
			TypeNativeNode type_void;

			TypeNativeNode type_abilcmd;
			TypeNativeNode type_actor;
			TypeNativeNode type_actorscope;
			TypeNativeNode type_aifilter;
			TypeNativeNode type_animtarget;
			TypeNativeNode type_bank;
			TypeNativeNode type_bool;
			TypeNativeNode type_byte;
			TypeNativeNode type_camerainfo;
			TypeNativeNode type_char;
			TypeNativeNode type_color;
			TypeNativeNode type_doodad;
			TypeNativeNode type_fixed;
			TypeNativeNode type_handle;
			TypeNativeNode type_int;
			TypeNativeNode type_marker;
			TypeNativeNode type_order;
			TypeNativeNode type_playergroup;
			TypeNativeNode type_point;
			TypeNativeNode type_region;
			TypeNativeNode type_revealer;
			TypeNativeNode type_sound;
			TypeNativeNode type_soundlink;
			TypeNativeNode type_string;
			TypeNativeNode type_text;
			TypeNativeNode type_timer;
			TypeNativeNode type_transmissionsource;
			TypeNativeNode type_trigger;
			TypeNativeNode type_unit;
			TypeNativeNode type_unitfilter;
			TypeNativeNode type_unitgroup;
			TypeNativeNode type_unitref;
			TypeNativeNode type_wave;
			TypeNativeNode type_waveinfo;
			TypeNativeNode type_wavetarget;

			Types(Compiler &compiler);
	};
};
