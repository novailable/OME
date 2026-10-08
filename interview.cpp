#include <iostream>
#include <string>
#include <utility>
#include <tuple>

class Node
{
    int i = 0;
    std::string s;

public:
    Node() = default;

    Node(int value, std::string str)
        : i(value), s(std::move(str))
    {
        std::cout << "Normal constructor called\n";
    }

    // Move constructor
    Node(Node&& node)
        : i(node.i),
          s(std::move(node.s))
    {
        std::cout << "Move constructor called\n";
    }

    // Move assignment operator
    Node& operator=(Node&& node)
    {
        std::cout << "Move assignment called\n";

        if (this != &node)
        {
            i = node.i;
            s = std::move(node.s);
        }

        return *this;
    }

    void print() const
    {
        std::cout << "i = " << i
                  << ", s = \"" << s << "\"\n";
    }
};

void	move_ocf()
{
	 std::cout << "Creating node1:\n";
    Node node1(10, "Hello");

    std::cout << "\nMove constructing node2 from node1:\n";
    Node node2(std::move(node1));

    std::cout << "\nnode2:\n";
    node2.print();

    std::cout << "\nCreating node3:\n";
    Node node3(20, "World");

    std::cout << "\nMove assigning node2 to node3:\n";
    node3 = std::move(node2);

    std::cout << "\nnode3:\n";
    node3.print();
}

void	lambda()
{
	int	counter = 0;
	auto a = [&counter]{std::cout << " inside " << counter << std::endl;};
	auto b = [=] {std::cout << "b: " << counter << std::endl;};
	a(), b();
	std::cout << "outside" << &counter << std::endl;
	int c = 0, d = 0;
	auto [x, y, z] = [c, d]{return std::tuple{c + 1, d + 1, d + 2};}();
	std::cout << "x: " << x << ", y: " << y << ", z: " << z << std::endl;
	

}

int main()
{ 
	// move_ocf();
	lambda();
    return 0;
}