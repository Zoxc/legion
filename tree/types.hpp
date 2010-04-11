#pragma once
#include "../common.hpp"
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
			ArrayType *first_array;

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

			Kind kind;

			Type(Kind kind) : indirect(0), first_array(0), kind(kind)
			{
			}

			Type *get_indirect(ValidationArgs &args);
			static Type *resolve(Type *type);
			void compatible(ValidationArgs &args, ExpressionNode *node);
			void compatible(ValidationArgs &args, Type *type, ExpressionNode *node);

			virtual std::string string(bool show_typedef = true) = 0;

			virtual bool compatible(ValidationArgs &args, Type *other)
			{
				return this == other || this == resolve(other);
			}

			bool exact(Type *other)
			{
				if(!this || !other)
					return true;

				return is_exact(other);
			}
	};

	class NativeType:
		public Type
	{
		public:
			String *name;

			std::set<Type *> implicit_conversions; //TODO: Remove usage of set

			std::string string(bool show_typedef)
			{
				return name->string();
			}

			bool compatible(ValidationArgs &args, Type *other)
			{
				Type *type = resolve(other);

				if(this == type)
					return true;

				return implicit_conversions.find(type) != implicit_conversions.end();
			}

			NativeType() : Type(Type::NATIVE_TYPE) {}
	};

	struct TypeNativeNode:
		public Node
	{
		NativeType type;

		NodeType node_type()
		{
			return Node::TYPE_NATIVE_NODE;
		}

		Type *get_type(Document &document, SymbolList &stack)
		{
			return &type;
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

			std::string string(bool show_typedef)
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
			bool is_exact(Type *other)
			{
				if(other->kind != Type::FUNCTION_TYPE)
					return false;

				FunctionType *other_func = (FunctionType *)other;

				if(other_func->params.size != params.size)
					return false;

				if(!other_func->returns->exact(returns))
					return false;

				List<Parameter>::Iterator j = params.begin();

				for(List<Parameter>::Iterator i = other_func->params.begin(); i; i++, j++)
					if(!i().type->exact(j().type))
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

			bool compatible(ValidationArgs &args, Type *other)
			{
				if(!base)
					return true;

				return this == other || base->compatible(args, other);
			}
	};

	class ArrayType:
		public Type
	{
		public:
			ArrayType(Type *base, size_t size) : Type(Type::ARRAY_TYPE), base(base), size(size) {}

			Type *base;
			size_t size;
			ArrayType *next_array;

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

			bool compatible(ValidationArgs &args, Type *other);
	};

	class Compiler;

	class Types
	{
		private:
			void declare(Compiler &compiler, const char *name, TypeNativeNode &type, bool declare);

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

	Type *lookup_type(Document &document, SymbolList &stack, String *name, Range *range);
};
