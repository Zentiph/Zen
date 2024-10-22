# Syntax
Zen's syntax is aimed to have a similar feeling to Python and C++.

```
// comments look like this.

/.
   long comments
   look like this.
./

int my_var = 10 // variables look like this.

// standard operators:
1 + 2
4 - 2
5 * 2
3 / 1 // floor div not included as part of base syntax

3 > 2
2 < 4
9 >= 9
0 <= 2
1 == 1
2 != -2

// arrays look like this.
my_array = [1, 2, 3, 4]

// functions look like this.
fn my_func(int x, int y, int div=1) -> float {
    return x * y / div
}

// lambda functions look like this.
fn take_func(function[int] func, int x) -> int {
    return func(x) + 3
}

take_func(int x => x * x, 2)

// classes look like this.
class MyClass {
    constructor(int a) { // args can be added here if wanted
        int this.a = a   // this references this class instance
    }

    string attribute = "hi"

    fn method(int b) {
        this.a += b
    }

    @operator_overload("+")
    fn add(MyClass other) {
        return MyClass(this.a + other.a)
    }
}

class Inherited(MyClass) {
    constructor(int a) {
        super.constructor(a)
    }
}

Inherited instance = Inherited(2)
instance.method(2) // -> instance.a = 4

// conditionals
if (1 == 2) {
    print("what")
} elseif (1 == -1) {
    print("seriously?")
} else {
    print("made it")
}

// for loops
array[int] arr = [1, 2, 3]
for (i) in (arr) {
    arr[i] = (i + 2) / 3
}

map[str, int] my_map = ["Abby": 20, "Brian": 24]
for (k, v) in (my_map) {
    print(k, "is", v, "years old")
}

// while loops
while (true) {
    print("stuck")
}

int count = 0
while (count < 5) {
    print(count)
    count += 1
}
```

# Semantics
Variables and functions work like in any other language, and classes take an inspiration from Python.

# Features

## Data Types
| Zen         | Python    | C              |
| ----------- | --------- | -------------- |
| int         | int       | long long      |
| float       | float     | double double  |
| complex     | complex   | double complex |
| string      | str       | char[]         |
| array       | list      | malloc         |
| lockedarray | tuple     | N/A            |
| map         | dict      | N/A            |
| lockedmap   | N/A       | N/A            |
| set         | set       | N/A            |
| lockedset   | frozenset | N/A            |
| bool        | bool      | _Bool          |
| null        | NoneType  | NULL           |
```
// int
int x = 3
int y = -2
int z = 9999999999

// float
float x = 3.2
float y = -0.4
float z = 1.0

// complex
complex x = 1i
complex y = 1 - 3i
complex z = 0 + 0j

// string
string greeting = "hello"
string char = "a"
string empty = ""
greeting[0] // indexing

// array
array[] of_any = [1, 2.0, "hi"]
array[int] of_int = [1, 2, 3]
// and so on...
of_any[0]      // indexing
of_any[0] = -1 // setting

// lockedarray
lockedarray locked = (1, 2, 3)
locked[0] // indexing

// map
map[] of_any = [1: "one", 2.0: null]
map[string] string_to_any = ["Abby": 20, "Brian": 23]
map[int, string] int_to_string = [1: "one", 2: "two"]
map[] empty = map()
of_any[2.0]       // indexing
of_any[3] = 3.0 // setting

// lockedmap
lockedmap[] of_any = [1: "one", 2.0: null]
of_any[2.0] // indexing

// set
set{} of_any = {1, 2.0, "hi"}
set{string} of_string = {"hi", "a", "123"}
set{} empty = {}

// lockedset
lockedset{} of_any = lockedset(1, 2.0, "hi)
lockedset{string} of_string = {"a", "b", "c"}

// bool
bool flag = true
bool flag2 = false
flag + 1 == 2 // -> true
// true = 1, false = 0

// null
nulltype none = null
```

## Errors
Some of the builtin error types include:
* Error
  * Base error class
* FailedAssertionError
  * When `assert` is given `false`
* MissingImportError
  * When an imported item cannot be found
* MissingModuleError
  * When an imported module cannot be found
* MissingIndexError
  * When an indexable object is indexed at an invalid location
* UserExit
  * When the user exits the program via keyboard escape code
* UndefinedNameError
  * When an undefined name is referenced
* OSFailureError
  * When a system process runs into an error
* OverflowError
  * When a math calculation returns a number too large to represent
* MaxRecursionDepthError
  * When the maximum recursion depth is reached
* SyntaxError
  * When a syntax mistake is made
* Exit
  * Used to stop the interpreter
* InvalidTypeError
  * When an invalid type is used with a function or operation
* InvalidValueError
  * When an invalid value is passed to a function or operation

```
// syntax for error handling:
protect {
    raise SomeError("uh oh")
} handle SomeError, OtherError {
    print("An error occurred!")
}
```
