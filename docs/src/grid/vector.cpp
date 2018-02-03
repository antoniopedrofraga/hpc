#include "vector.h"

// CONSTRUCTORS
/*=
* Default constructor (empty vector)
*/
Vector::Vector() : std::vector<double>() {}


/*
* Alternate constructor - creates a vector of a given size
*/
Vector::Vector(int Num) : std::vector<double>()
{
	// set the size
	(*this).resize(Num);

	// initialise with zero
	std::size_t i;
	for (i = 0; i < size(); i++) (*this)[i] = 0.0;
}

/*
* Copy constructor
*/
Vector::Vector(const Vector& copy) : std::vector<double>()
{
	(*this).resize(copy.size());
    // copy the data members (if vector is empty then num==0)
	std::size_t i;
    for (i=0; i<copy.size(); i++) (*this)[i]=copy[i]; 
}

/*
* Copy constructor from different type
*/
Vector::Vector(std::vector<double> vec) : std::vector<double>() {
	(*this).resize(vec.size());
    // copy the data members (if vector is empty then num==0)
	std::size_t i;
    for (i=0; i<vec.size(); i++) (*this)[i]=vec[i];
}

/*
* accessor method - get the size
*/
int Vector::getSize() const
{
	return size();
}

/*
* aux method - pushes a value to the begining and ending of a vector
*/
void Vector::push_front_back(double value) {
	this->insert(this->begin(), value);
	this->push_back(value);
}

/*
* aux method - pushes a value to the vector
*/
void Vector::push(double value) {
	this->push_back(value);
}

/*
* aux method - finds a given value
*/
int Vector::find(double value) {
	std::vector<double>::iterator it = std::find((*this).begin(), (*this).end(), value);
	if (it != (*this).end()) {
		return std::distance((*this).begin(), it);
	} else {
		return -1;
	}
}

// OVERLOADED OPERATORS
/*
* Operator= - assignment
*/
Vector& Vector::operator=(const Vector& copy)
{
	if (this != &copy) {
        (*this).resize(copy.size());
		std::size_t i;
    	for (i=0; i<copy.size(); i++) (*this)[i] = copy[i]; 
    }
    return *this;
}


// COMPARISON
/*
* Operator== - comparison
*/
bool Vector::operator==(const Vector& v) const
{
	if (size() != v.size()) throw std::invalid_argument("incompatible vector sizes\n");
	std::size_t i;
	for (i = 0; i<size(); i++) 	if (fabs((*this)[i] - v[i]) > 1.e-07) { return false; }
	return true;
}

// NORMS
/*
* 1 norm
*/
double Vector::one_norm() const {
	size_t n = size();
	double result = 0.0;
	for (size_t index = 0; index < n; index++) {
		result += std::abs((*this)[index]);
	}
	return result;
}

/*
* 2 norm
*/
double Vector::two_norm() const
{
	size_t n = size();
	double result = 0.0;
	for (size_t index = 0; index < n; index++) {
		result += pow((*this)[index], 2);
	}
	return sqrt(result);
}

/*
* uniform (infinity) norm
*/
double Vector::uniform_norm() const
{
	size_t n = size();
	double result = -DBL_MAX;
	for (size_t index = 0; index < n; index++) {
		double value = std::abs((*this)[index]);
		if (value > result) result = value;
	}
	return result;
}


// INPUT AND OUTPUT
/*
* keyboard input , user friendly
*/
std::istream& operator>>(std::istream& is, Vector& v)
{
	if (!v.size()) {
		int n;

		std::cout << "input the size for the vector" << std::endl;
		is >> n;
		//check input sanity
		if(n < 0) throw std::invalid_argument("read error - negative vector size");

		// prepare the vector to hold n elements
		v = Vector(n);
	}
	// input the elements
	std::cout << "input "<< v.size() <<" vector elements" << std::endl;
	std::size_t i;
	for (i=0; i<v.size(); i++) is >> v[i];

    // return the stream object
    return is;
}

/*
* file input - raw data, compatible with file writing operator
*/
std::ifstream& operator>>(std::ifstream& ifs, Vector& v) 
{
    int n;

    // read size from the file
    ifs >> n;
    //check input sanity
    if(n < 0) throw std::invalid_argument("file read error - negative vector size");

    // prepare the vector to hold n elements
    v = Vector(n);

    // input the elements
    for (int i=0; i<n; i++) ifs >> v[i];

    // return the stream object
    return ifs;
}

/*
* screen output, user friendly
*/
std::ostream& operator<<(std::ostream& os, const Vector& v)
{
    if (v.size() > 0) {
		std::size_t i;
        for (i=0; i<v.size(); i++) os << v[i]  << " ";
        os << std::endl;
    }
    else
    {
        os << "Vector is empty." << std::endl;
    }
    return os;
}

/*
* file output - raw data, comaptible with file reading operator
*/
std::ofstream& operator<<(std::ofstream& ofs, const Vector& v)
{
    //put vector size in first line (even if it is zero)
    ofs << v.size() << std::endl;
    //put data in second line (if size==zero nothing will be put)
	std::size_t i;
    for (i=0; i<v.size(); i++) ofs << v[i]  <<  " ";
    ofs << std::endl;
    return ofs;
}


