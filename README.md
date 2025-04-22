# The Zen Programming Language
Zen is a statically-typed, compiled programming language developed from scratch as a personal learning project. It aims to blend the type safety and structure of Java with the simplicity and developer-friendly nature of Python, making for a truly zen-like coding experience.

![GitHub commit activity](https://img.shields.io/github/commit-activity/m/zentiph/zen)
![GitHub last commit](https://img.shields.io/github/last-commit/zentiph/zen)

## Inspirations
Zen primarily takes inspiration from Python and Java, however the idea for this project was inspired by [Pinecone](https://github.com/wmww/Pinecone?tab=readme-ov-file).

## Example
Here is a demo program performing the FizzBuzz algorithm (may be subject to change as the language evolves):
```
// fizzbuzz.zen

fn fizzbuzz(int start, int end) {
    int num = start
    while (num < end) {
        string str = ""
        if (num % 3 == 0) {
            str += "Fizz"
        }
        if (num % 4 == 0) {
            str += "Buzz"
        }
        if (num % 4 != 0 and num % 3 != 0) {
            str += num
        }
        print(str)
    }
}

fizzbuzz(0, 20)
```
For more examples, check out [examples](examples/) or [tutorials](tutorials/).

## Current State
Zen currently supports:
* Nothing :D

## Contributing
This project is not yet big enough or organized enough to accept contributions, but if you have an inquiry, request, or new feature idea, reach out to me at [zentiphdev@gmail.com](mailto:zentiphdev@gmail.com).
