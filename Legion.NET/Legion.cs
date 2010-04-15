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
        private static extern IntPtr legion_document_create(IntPtr compiler, String name);
        
        [DllImport("liblegion.dll")]
        private static extern void legion_document_destroy(IntPtr document);

        [DllImport("liblegion.dll")]
        private static extern String legion_document_filename(IntPtr document);

        [DllImport("liblegion.dll")]
        private static extern void legion_document_load_data(IntPtr document, byte[] data, uint length);

        [DllImport("liblegion.dll")]
        private static extern bool legion_document_load_file(IntPtr document, String filename);

        [DllImport("liblegion.dll")]
        private static extern void legion_document_execute(IntPtr document, Stage stage);

        [DllImport("liblegion.dll")]
        private static extern IntPtr legion_message_first(IntPtr document);

        [DllImport("liblegion.dll")]
        private static extern IntPtr legion_message_next(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern String legion_message_string(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern Severity legion_message_severity(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern uint legion_message_start(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern uint legion_message_stop(IntPtr message);

        [DllImport("liblegion.dll")]
        private static extern uint legion_message_line(IntPtr message);
        
        private IntPtr _document;

        public Document(Compiler compiler, String name)
        {
            _document = legion_document_create(compiler._compiler, name);
        }

        public void Free()
        {
            if (_document == IntPtr.Zero)
                return;

            legion_document_destroy(_document);

            _document = IntPtr.Zero;
        }

        public bool LoadFile(String filename)
        {
            return legion_document_load_file(_document, filename);
        }

        public void Load(byte[] data)
        {
            legion_document_load_data(_document, data, (uint)data.Length);
        }

        public void Load(String data)
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
                result.Add(new Message(legion_message_string(message), legion_message_severity(message), legion_message_start(message), legion_message_stop(message), legion_message_line(message)));

                message = legion_message_next(message);
            }

            return result;
        }

        ~Document()
        {
            Free();
        }
    }
    
    public class Message
    {
        private String _message;
        private Severity _severity;
        private uint _start;
        private uint _stop;
        private uint _line;

        public Message(String message, Severity severity, uint start, uint stop, uint line)
        {
            _message = message;
            _severity = severity;
            _start = start;
            _stop = stop;
            _line = line;
        }

        public override String ToString()
        {
            return _message;
        }

        public Severity Severity
        {
            get
            {
                return _severity;
            }
        }

        public uint Start
        {
            get
            {
                return _start;
            }
        }

        public uint Stop
        {
            get
            {
                return _stop;
            }
        }

        public uint Line
        {
            get
            {
                return _line;
            }
        }

    }
}
