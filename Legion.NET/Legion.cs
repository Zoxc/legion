using System;
using System.Text;
using System.Collections.Generic;
using System.Runtime.InteropServices;

namespace Legion
{
    public class Compiler
    {
        [DllImport("liblegion.dll")]
        private static extern IntPtr legion_compiler_create();

        [DllImport("liblegion.dll")]
        private static extern void legion_compiler_destroy(IntPtr compiler);

        public IntPtr _compiler;
        public object Tag;

        public Compiler()
        {
            _compiler = legion_compiler_create();
        }

        public void Free()
        {
            if (_compiler == IntPtr.Zero)
                return;

            legion_compiler_destroy(_compiler);

            _compiler = IntPtr.Zero;
        }

        ~Compiler()
        {
            Free();
        }
    }

    public enum Stage : int
    {
        Parse,
        Declare,
        Validate,
        Stages
    };


    public enum Severity : int
    {
        Hint,
        Warning,
        Error,
        Severities
    };

    public class Document
    {
        [DllImport("liblegion.dll")]
        private static extern IntPtr legion_document_create(IntPtr compiler, string name);
        
        [DllImport("liblegion.dll")]
        private static extern void legion_document_destroy(IntPtr document);

        [DllImport("liblegion.dll")]
        private static extern string legion_document_filename(IntPtr document);

        [DllImport("liblegion.dll")]
        private static extern void legion_document_load_data(IntPtr document, byte[] data, uint length);

        [DllImport("liblegion.dll")]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool legion_document_load_file(IntPtr document, string filename);

        [DllImport("liblegion.dll")]
        private static extern void legion_document_execute(IntPtr document, Stage stage);

        [DllImport("liblegion.dll")]
        private static extern IntPtr legion_message_first(IntPtr document);

        [DllImport("liblegion.dll")]
        private static extern IntPtr legion_message_next(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern IntPtr legion_include_first(IntPtr document);

        [DllImport("liblegion.dll")]
        private static extern IntPtr legion_include_next(IntPtr include);

        private IntPtr _document;
        private GCHandle pin;

        public object Tag;

        public Document(Compiler compiler, string name)
        {
            _document = legion_document_create(compiler._compiler, name);
        }

        public void Free()
        {
            if (_document != IntPtr.Zero)
            {
                legion_document_destroy(_document);
                _document = IntPtr.Zero;
            }

            if (pin.IsAllocated)
                pin.Free();
        }

        public bool LoadFile(string filename)
        {
            return legion_document_load_file(_document, filename);
        }

        public void Load(byte[] data)
        {
            if(pin.IsAllocated)
                pin.Free();

            pin = GCHandle.Alloc(data, GCHandleType.Pinned);

            legion_document_load_data(_document, data, (uint)data.Length);
        }

        public void Load(string data)
        {
            Load(Encoding.UTF8.GetBytes(data));
        }

        public void Run()
        {
            Run(Stage.Parse);
            Run(Stage.Declare);
            Run(Stage.Validate);
        }

        public void Run(Stage stage)
        {
            legion_document_execute(_document, stage);
        }

        public List<Message> GetMessages()
        {
            List<Message> result = new List<Message>();
            IntPtr message = legion_message_first(_document);

            while(message != IntPtr.Zero)
            {
                result.Add(new Message(message));

                message = legion_message_next(message);
            }

            return result;
        }

        public List<Include> GetIncludes()
        {
            List<Include> result = new List<Include>();
            IntPtr include = legion_include_first(_document);

            while (include != IntPtr.Zero)
            {
                result.Add(new Include(include));

                include = legion_include_next(include);
            }

            return result;
        }
        ~Document()
        {
            Free();
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct String
    {
        IntPtr Bytes;
        public uint Length;

        public override string ToString()
        {
            byte[] data = new byte[Length];

            Marshal.Copy(Bytes, data, 0, (int)Length);

            return Encoding.UTF8.GetString(data);
        }
    }

    public class Include
    {
        [DllImport("liblegion.dll")]
        private static extern IntPtr legion_include_filename(IntPtr include);

        [DllImport("liblegion.dll")]
        [return: MarshalAs(UnmanagedType.I1)]
        private static extern bool legion_include_included(IntPtr include);

        [DllImport("liblegion.dll")]
        private static extern void legion_include_report(IntPtr include);

        public string Path;
        public bool Included;
        private IntPtr include;

        public Include(IntPtr include)
        {
            this.include = include;
            Included = legion_include_included(include);
            IntPtr path = legion_include_filename(include);
            Path = Marshal.PtrToStructure(path, typeof(String)).ToString();
        }

        public void Report()
        {
            legion_include_report(include);
        }
    }

    public class Message
    {
        [DllImport("liblegion.dll")]
        private static extern string legion_message_string(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern Severity legion_message_severity(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern uint legion_message_start(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern uint legion_message_stop(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern uint legion_message_line(IntPtr message);

        private string message;
        private Severity severity;
        private uint start;
        private uint stop;
        private uint line;
        public object Tag;

        public Message(IntPtr message)
        {
            this.message = legion_message_string(message);
            severity = legion_message_severity(message);
            start = legion_message_start(message);
            stop = legion_message_stop(message);
            line = legion_message_line(message);
        }

        public override string ToString()
        {
            return message;
        }

        public Severity Severity
        {
            get
            {
                return severity;
            }
        }

        public uint Start
        {
            get
            {
                return start;
            }
        }

        public uint Stop
        {
            get
            {
                return stop;
            }
        }

        public uint Line
        {
            get
            {
                return line;
            }
        }

    }
}
