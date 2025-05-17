#include <algorithm>
#include <iostream>
#include <vector>

#include <license.h>

void PrintKey(const std::vector<unsigned char> &key) {
    std::for_each(key.begin(), key.end(),
                  [](unsigned char c) { std::cout << int(c) << " "; });
    std::cout << std::endl;
}

int main() {
    std::vector<unsigned char> pk, sk;

    lic::GenerateKeyPair(pk, sk);

    std::cout << "Public key: " << std::endl;
    PrintKey(pk);

    std::cout << "Secret key: " << std::endl;
    PrintKey(sk);

    return 0;
}
