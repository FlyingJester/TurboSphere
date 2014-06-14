An ad-hoc list of stylistic conventions I would like to have in the TurboSphere source.

* Order getters before setters. Likewise for related definitions and implementations.
* The following commenting style should be used:

/////
// Five slashes on the first line, followed by two slashes and a space on every line
// after that.
//
// Blank comment lines are good for breaking sections up. Leave a trailing empty comment
// line at the end of multi-line comments.
//

// One liners without the five leading slashes are OK for explaining local processes.

* Use modified banner style indents:

int Foo(bool isbar, bool isbaz, bool islolcat){    if(isBar)    {        bar();        return 1;    }    else if(isbaz || islolcat){        return 0;
    }
    else
        return 2;}

* Use CamelCase for global constructions: class and struct names, namespaces, etc.
* Use dromedaryCase for members.
* Use all lowercase (or lower_case) for scope-local variables.
* You don’t have to limit every file to a single class or function, but try not have
   too much in any one file.
* Do not place vim formatting information in any files.
* Try to keep lines under 120 chars long.