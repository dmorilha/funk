funk
====

funk - quick parser to extract parameters out of functions

Once upon a time, I faced a problem of extracting a fixed parameter out of function calls throughtout thousands of javascript files.

I basically had two hypothesis:

 - The first approach was to run the subjected code into a vm, modifying the function I desired to extract the parameter from. However this approach prooved to be complicated once it turned out to be difficult to isolate other parts of the code from running.

 - The second approach is funk, a quick parser which scans the source code. Please be aware that this approach is limited (it ignores variables)

```
make funk
./funk function-name function-argument-number [ function-name function-argument-number ... ]
```
