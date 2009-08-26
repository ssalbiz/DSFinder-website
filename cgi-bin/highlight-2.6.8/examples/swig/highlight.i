%module highlight

// dont know why SWIG needs it...
%include std_string.i
%apply const std::string& { const string& };


%{
#include "../../src/codegenerator.h"
%}

%include "../../src/enums.h"
%include "../../src/languagedefinition.h"
%include "../../src/codegenerator.h"
