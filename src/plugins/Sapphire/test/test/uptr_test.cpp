#include <cstdio>
#include <memory>
#include <vector>

using namespace std;

struct A {

    A(int l) {
        lol = l;
    }
    ~A() {
        printf("%i\n", lol);
    }

    int lol;

};

typedef unique_ptr<A> aptr;
typedef vector<aptr> uav;

class B {
public:
    B() {
        vec = uav();
    }
    ~B() {}
    inline void push_back(aptr &ptr) {
        vec.push_back(move(ptr));
    }

private:
    uav vec;
};

int main(int argc, char *argv[]) {


    B* b = new B();

    uav vec = uav();
    {
        vec.push_back(move(aptr(new A(0))));
        vec.push_back(move(aptr(new A(1))));
        vec.push_back(move(aptr(new A(2))));
        vec.push_back(move(aptr(new A(3))));

        b->push_back(vec[0]);
    }

    aptr a = aptr(new A(10));

    b->push_back(a);

    uav vec2 = move(vec);

    vec2.push_back(move(aptr(new A(4))));

    delete b;

    return 1;

}
