# The Zen Programming Language
This programming language has been built from scratch primarily as a learning project by a solo developer. Zen is a static-typed interpreted language. (WHAT???)

## Why
Because I wanted to learn how programming languages work and I'm an asshole who wanted to name a programming language after himself (shameless self-promo).

## Inspirations
The Zen programming language is primarily inspired by Python and C++ syntax. The idea for this project was inspired by [Pinecone](https://github.com/wmww/Pinecone?tab=readme-ov-file).

## Example
Here is a demo program performing the FizzBuzz algorithm:
```
fn fizzbuzz(int start, int end) {
    int num = start
    while (num < end) {
        string str = ""
        if (num % 3 == 0) {
            string += "Fizz"
        }
        if (num % 4 == 0) {
            string += "Buzz"
        }
        if (num % 4 != 0 and num % 3 != 0) {
            string += str(num)
        }
        print(string)
    }
}

fizzbuzz(0, 20)
```
For more examples, check out [examples](examples/) or [tutorials](tutorials/).

## Current State
Zen currently supports:
* Nothing :D

## Contributing
This project is not yet big enough or organized enough to accept contributions, but if you have an inquiry, request, or new feature idea, reach out to me at zentiphdev@gmail.com.