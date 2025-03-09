# Syntax
Zen's syntax is aimed to have a similar feeling to Python and Java.
The goal is to make it fast to write while maintaining type safety.

```
// comments look like this.

/.
 long comments
 look like this.
./

/..
 . or this,
 . if you're feeling fancy.
 . it doesn't matter,
 . as long as it starts with "/." and ends with "./"
../

// ------------------------------------------------ //

// variables look like this.
int my_var = 10

// standard operators:
1 + 2
4 - 2
5 * 2
3 / 1
// unlike python, floor div is
// not included as part of the base syntax

// in-place operators:
1 += 2
4 -= 2
5 *= 2
3 /= 1
1++
2--

// ------------------------------------------------ //

// comparison operators
3 > 2
2 < 4
9 >= 9
0 <= 2
1 == 1
2 != -2

// conditionals
if 1 == 2 {
    print("what")
} elseif 1 <= -1 {
    print("seriously?")
} else {
    print("made it")
}

// single-line if statement
if true {print("true")} else {print("false")}

// ------------------------------------------------ //

// for loops
array<int> arr = [1, 2, 3]
for int i in arr {
    arr[i] = (i + 2) / 3
}
for int i in new counter(10) {
    print(i)
}

// ------------------------------------------------ //

// while loops
while true {
    print("stuck")
}

int count = 0
while count < 5 {
    print(count)
    count += 1
}

// ------------------------------------------------ //

// native arrays look like this.
array<int> my_array = [1, 2, 3, 4]
array<int> my_array = new array[4]

// ------------------------------------------------ //

// functions look like this.
// default parameter values are supported,
// and any parameter with a default value can
// be treated as a keyword argument.
fn my_func(int x, int y, int div=1) -> float {
    return x * y / div
}
my_func(2, 3)
my_func(2, 3, 5)
my_func(2, 3, div=5)

// a function with no return arrow indicates it returns nothing
fn return_nothing(array<int> arr) {
    arr[0] += 1
}

// function overloading is possible
fn square(int x) -> int {
    return x * x
}
fn square(float x) -> float {
    return x * x
}

// lambda functions look like this.
fn take_func((int => int) func, int x) -> int {
    return func(x) + 3
}

take_func(int x => x * x, 2)

// ------------------------------------------------ //

// classes look like this.
class MyClass {
    public {
        // public fields
        string message = "hi"

        // constructor
        MyClass(int a) {
            this.a = a   // this references this class instance
        }

        fn method(int b) {
            this.a += b
        }
    }

    private {
        // private fields
        int a

        // operator overloading
        @operator<+>
        fn add(MyClass other) {
            return MyClass(this.a + other.a)
        }
        @operator<+>
        fn add(int other) {
            return MyClass(this.a + other)
        }
    }
}

// generics
class Collection<T> {
    public {
        Collection() {
            // ...
        }

        T get(int index) {
            // ...
        }
    }
}

// abstract classes
abstract class MyAbstract {
    public {
        MyAbstract(int c) {
            this.c = c
        }

        fn method(int d) {
            this.c -= d
        }
    }

    private {int c}
}

// inheritance
class Inherited(MyClass) {}

Inherited instance = Inherited(2)
instance.method(2) // -> instance.a = 4

// ------------------------------------------------ //

// built-in type example(s)
map<string, int> my_map = ["Abby": 20, "Brian": 24]
for string k, int v in my_map {
    print(k, "is", v, "years old")
}
```

# Semantics
Variables and functions work like in any other language, and classes take an inspiration from Python.

# Features

## Data Types / Data Structures
| Zen           | Python      | Java                             |
| ------------- | ----------- | -------------------------------- |
| `int`         | `int`       | `long long`                      |
| `float`       | `float`     | `double double`                  |
| `complex`     | `complex`   | `std::complex<double double>`    |
| `string`      | `str`       | `std::string`                    |
| `counter`     | `range`     | -                                |
| `array<E>`    | `list`      | `std::vector<T>`                 |
| `tuple<E>`    | `tuple`     | `std::tuple<T1, T2, ...>`        |
| `map<K, V>`   | `dict`      | `std::unordered_map<Key, Value>` |
| `set<E>`      | `set`       | `std::unordered_set<T>`          |
| `bool`        | `bool`      | `bool`                           |
| `null`        | `NoneType`  | `std::nullptr_t`                 |
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
complex z = 0 + 0i

// string
string greeting = "hello"
string char = "a"
string empty = ""
greeting[0] // indexing

// counter
for int i in new counter(2, 5) {
    print(i)
}

// array
array<object> of_any = [1, 2.0, "hi"]
array<int> of_int = [1, 2, 3]
// and so on...
of_any[0]      // indexing
of_any[0] = -1 // setting

// tuple
tuple<int> locked = (1, 2, 3)
locked[0] // indexing

// map
map<object, object> of_any = [
    1: "one",
    2.0: null
]
map<string, object> string_to_any = [
    "Abby": 20,
    "Brian": 23
]
map<int, string> int_to_string = [
    1: "one",
    2: "two"
]
map<int> empty = new map()
of_any[2.0]     // indexing
of_any[3] = 3.0 // setting

// set
set<object> of_any = {1, 2.0, "hi"}
set<string> of_string = {"hi", "a", "123"}
set<int> empty = new set()

// bool
bool flag = true
bool flag2 = false
if flag and flag2 {
    print("worked")
}

// null
map<> blank = null
```

## Errors
Some of the builtin error types include:
* `Error`
  * Base error class
* `FailedAssertionError`
  * When `assert` is given `false`
* `MissingImportError`
  * When an imported item cannot be found
* `MissingModuleError`
  * When an imported module cannot be found
* `InvalidIndexError`
  * When an indexable object is indexed at an invalid location
* `UserExit`
  * When the user exits the program via keyboard escape code
* `UndefinedNameError`
  * When an undefined name is referenced
* `OSError`
  * When a system process runs into an error
* `OverflowError`
  * When a math calculation returns a number too large to represent
* `MaxRecursionDepthError`
  * When the maximum recursion depth is reached
* `SyntaxError`
  * When a syntax mistake is made
* `InvalidTypeError`
  * When an invalid type is used with a function or operation
* `InvalidValueError`
  * When an invalid value is passed to a function or operation

```
// syntax for error handling/throwing:
try {
    throw SomeError("uh oh")
} catch (SomeError, OtherError) {
    print("An error occurred!")
} finally {
    print("reached the end")
}
```

## Operators
Here's a list of all operators in Zen (including planned):
| Operator | Type       | Functionality                                       | Example     |
| -------- | ---------- | --------------------------------------------------- | ----------- |
| `=`      | Binary     | Assignment                                          | `int x = 3` |
| `+`      | Binary     | Add two objects                                     | `x + y`     |
| `-`      | Binary     | Subtract two objects                                | `x - y`     |
| `*`      | Binary     | Multiply two objects                                | `x * y`     |
| `/`      | Binary     | Divide two objects                                  | `x / y`     |
| `%`      | Binary     | Modulus of two objects                              | `x % y`     |
| `+=`     | Binary     | Add two objects in-place                            | `x += y`    |
| `-=`     | Binary     | Subtract two objects in-place                       | `x -= y`    |
| `*=`     | Binary     | Multiply two objects in-place                       | `x *= y`    |
| `/=`     | Binary     | Divide two objects in-place                         | `x /= y`    |
| `%=`     | Binary     | Modulus two objects in-place                        | `x %= 2`    |
| `==`     | Comparison | Check if two objects are equal                      | `x == y`    |
| `!=`     | Comparison | Check if two objects are not equal                  | `x != y`    |
| `>`      | Comparison | Check an object is greater than another             | `x > y`     |
| `<`      | Comparison | Check an object is less than another                | `x < y`     |
| `>=`     | Comparison | Check an object is greater than or equal to another | `x >= y`    |
| `<=`     | Comparison | Check an object is less than or equal to another    | `x <= y`    |
| `and`    | Logic      | Check if two expressions are true                   | `x and y`   |
| `or`     | Logic      | Check if one of two expressions are true            | `x or y`    |
| `not`    | Logic      | Reverse an expression's truthiness                  | `not x`     |
