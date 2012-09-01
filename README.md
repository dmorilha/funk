funk
====

funk - quick parser to extract parameters out of functions

I faced the problem of extracting a given (fixed) parameter out of a function throughtout hundred of javascript files.

I basically had two hypothesis:
 - The first one was to run the subjected code into a vm, modifying the function I desired to extract the parameter from. However this approach showed complicated once I couldn't isolate other parts of the code from running.
 - The second approach was funk, a quick parser which scans the source code.

make funk
./funk function-name function-argument-number [ function-name function-argument-number ... ]
