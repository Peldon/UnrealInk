using Ink.Runtime;
using System.Collections.Generic;
using System.Linq;

namespace InkGlue
{
    public class GlueVariablesState
    {
        internal GlueVariablesState(VariablesState inVariablesState)
        {
            _variablesState = inVariablesState;
        }

        public string[] GetVariables()
        {
            return _variablesState.ToArray();
        }

        public object GetVariable(string name)
        {
            return _variablesState[name];
        }

        public string GetListVariable(string name)
        {
            object variable = _variablesState[name];
            if (variable is InkList)
            {
                List<string> entries = new List<string>();
                foreach (var itemAndValue in (InkList)variable)
                    entries.Add(string.Format("{0};{1};{2}", itemAndValue.Key.originName, itemAndValue.Key.itemName, itemAndValue.Value));
                return string.Join(",", entries);
            }
            return "";
        }

        public void SetVariable(string name, object value)
        {
            _variablesState[name] = value;
        }

        VariablesState _variablesState;
    }
}