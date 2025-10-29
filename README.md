# The Zen Programming Language
Zen is a statically-typed, compiled programming language developed from scratch as a personal learning project. It aims to blend the type safety and structure of Java with the simplicity and developer-friendly nature of Python, making for a truly zen-like coding experience.

[![GitHub License](https://img.shields.io/github/license/zentiph/zen?style=flat-square&labelColor=0f0f0f)](https://github.com/Zentiph/zen/blob/main/LICENSE.md)
[![Contributors](https://img.shields.io/github/contributors/zentiph/zen?style=flat-square&labelColor=0f0f0f)](../../graphs/contributors)
<br/>
[![GitHub commit activity](https://img.shields.io/github/commit-activity/t/zentiph/zen?style=flat-square&labelColor=0f0f0f)](https://github.com/zentiph/zen/commits/main)
[![Last Commit](https://img.shields.io/github/last-commit/zentiph/zen?style=flat-square&labelColor=0f0f0f)](https://github.com/zentiph/zen/commits/main)
<br/>
![C Standard](https://img.shields.io/badge/std-C99-A8B9CC?style=flat-square&labelColor=0f0f0f)
![Formatter](https://img.shields.io/badge/style-clang--format-116880?style=flat-square&labelColor=0f0f0f)

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

## License
This project is licensed under the GNU General Public License Version 3. [View the license](LICENSE.md) for more information.
