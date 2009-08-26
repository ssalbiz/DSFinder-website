# Python SWIG module test script
#
# Import highlight.py, which is the interface for the _highlight.so module.
# See highlight.py for all available attributes and class members.
import highlight

def main():

  #get a generator instance (for HTML output)
  gen=highlight.CodeGenerator_getInstance(highlight.HTML);

  #initialize the generator with a colour theme and the language definition
  gen.initTheme("/usr/share/highlight/themes/kwrite.style");
  gen.initLanguage("/usr/share/highlight/langDefs/c.lang");

  #set some parameters
  gen.setIncludeStyle(1);
  gen.setEncoding("ISO-8859-1");

  #get output string
  print gen.generateCString("int main(int argc, char **argv) {\n"+
                            "  HighlightApp app;\n"+
                            "  return app.run(argc, argv);\n"+
                            "}\n");

  # clear the instance
  highlight.CodeGenerator_deleteInstance(gen);


main()
