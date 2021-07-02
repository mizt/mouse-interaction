cd "$(dirname "$0")"
cd ./

clang++ -std=c++17 -Wc++17-extensions -fobjc-arc -O3 -framework Cocoa ./MouseInteraction.mm -o ./MouseInteraction



