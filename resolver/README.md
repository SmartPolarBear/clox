# Design Notes

## Function Overloading

CLOXc supports function overloading, with following exception:

- When storing function object into variable, or using function object as return value, the last function defined is
  used.

For example:

```
fun func(a:integer):integer {
  return a+1;
}

fun func(a:floating):floating {
  return a+1.0;
}

var f=fun; // f is bound to the one with floating type

```

In such case, the last function defined, which is "the closest" to the variable definition, is used. Otherwise, the normal overloading resolution procedure is gone through.  