#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
using namespace std;

// ------------------------------------------------------------
// Function: Check if a string represents a valid double number
// ------------------------------------------------------------
bool isValidDouble(const string& s) {
    if (s.empty()) return false;

    int i = 0;
    if (s[0] == '+' || s[0] == '-') i++;

    bool hasDecimal = false;
    bool hasDigitsBefore = false;
    bool hasDigitsAfter = false;

    for (; i < (int)s.size(); i++) {
        if (s[i] == '.') {
            if (hasDecimal) return false; // multiple decimals
            hasDecimal = true;
        } 
        else if (isdigit(s[i])) {
            if (!hasDecimal) hasDigitsBefore = true;
            else hasDigitsAfter = true;
        } 
        else {
            return false; // invalid character
        }
    }

    if (hasDecimal)
        return (hasDigitsBefore && hasDigitsAfter);
    else
        return hasDigitsBefore;
}

// ------------------------------------------------------------
// Struct: Represents a parsed number split into components
// ------------------------------------------------------------
struct ParsedNumber {
    bool isNegative;
    string intPart;
    string fracPart;
};

// ------------------------------------------------------------
// Function: Parse a valid double string into its components
// ------------------------------------------------------------
ParsedNumber parseNumber(const string& s) {
    ParsedNumber num;
    num.isNegative = (s[0] == '-');

    int start = (s[0] == '+' || s[0] == '-') ? 1 : 0;
    size_t dotPos = s.find('.', start);

    if (dotPos == string::npos) {
        num.intPart = s.substr(start);
        num.fracPart = "";
    } else {
        num.intPart = s.substr(start, dotPos - start);
        num.fracPart = s.substr(dotPos + 1);
    }

    // remove leading zeros in integer part
    while (num.intPart.size() > 1 && num.intPart[0] == '0')
        num.intPart.erase(0, 1);

    // remove trailing zeros in fractional part
    while (!num.fracPart.empty() && num.fracPart.back() == '0')
        num.fracPart.pop_back();

    return num;
}

// ------------------------------------------------------------
// Helper: Compare two non-negative numeric strings (integer parts)
// ------------------------------------------------------------
int compareStrings(const string& a, const string& b) {
    if (a.size() > b.size()) return 1;
    if (a.size() < b.size()) return -1;
    if (a == b) return 0;
    return (a > b) ? 1 : -1;
}

// ------------------------------------------------------------
// Function: Add two non-negative integer strings
// ------------------------------------------------------------
string addStrings(const string& a, const string& b) {
    int carry = 0;
    string result = "";
    int i = a.size() - 1, j = b.size() - 1;

    while (i >= 0 || j >= 0 || carry) {
        int da = (i >= 0 ? a[i--] - '0' : 0);
        int db = (j >= 0 ? b[j--] - '0' : 0);
        int sum = da + db + carry;
        result.push_back((sum % 10) + '0');
        carry = sum / 10;
    }

    reverse(result.begin(), result.end());
    return result;
}

// ------------------------------------------------------------
// Function: Subtract two non-negative integer strings (a >= b)
// ------------------------------------------------------------
string subtractStrings(const string& a, const string& b) {
    int borrow = 0;
    string result = "";
    int i = a.size() - 1, j = b.size() - 1;

    while (i >= 0) {
        int da = a[i] - '0' - borrow;
        int db = (j >= 0 ? b[j] - '0' : 0);
        if (da < db) {
            da += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        result.push_back((da - db) + '0');
        i--; j--;
    }

    while (result.size() > 1 && result.back() == '0')
        result.pop_back();

    reverse(result.begin(), result.end());
    return result;
}

// ------------------------------------------------------------
// Normalize fractional parts for alignment
// ------------------------------------------------------------
void normalizeFractions(ParsedNumber& n1, ParsedNumber& n2) {
    while (n1.fracPart.size() < n2.fracPart.size()) n1.fracPart += '0';
    while (n2.fracPart.size() < n1.fracPart.size()) n2.fracPart += '0';
}

// ------------------------------------------------------------
// Function: Compare absolute values of two ParsedNumbers
// ------------------------------------------------------------
int compareAbsolute(const ParsedNumber& n1, const ParsedNumber& n2) {
    ParsedNumber a = n1, b = n2;
    normalizeFractions(a, b);

    int cmp = compareStrings(a.intPart, b.intPart);
    if (cmp != 0) return cmp;
    if (a.fracPart == b.fracPart) return 0;
    return (a.fracPart > b.fracPart) ? 1 : -1;
}

// ------------------------------------------------------------
// Function: Add or subtract two ParsedNumbers (handles signs)
// ------------------------------------------------------------
string addDoubles(const ParsedNumber& n1, const ParsedNumber& n2) {
    ParsedNumber a = n1, b = n2;
    normalizeFractions(a, b);

    string frac1 = a.fracPart, frac2 = b.fracPart;
    string int1 = a.intPart, int2 = b.intPart;

    string fracResult = "", intResult = "";
    int carry = 0;

    // CASE 1: same sign → normal addition
    if (a.isNegative == b.isNegative) {
        // fractional addition
        for (int i = (int)frac1.size() - 1; i >= 0; i--) {
            int sum = (frac1[i] - '0') + (frac2[i] - '0') + carry;
            fracResult.push_back((sum % 10) + '0');
            carry = sum / 10;
        }
        reverse(fracResult.begin(), fracResult.end());

        // integer addition
        intResult = addStrings(int1, int2);
        if (carry) intResult = addStrings(intResult, "1");

        string result = intResult;
        if (!fracResult.empty()) result += "." + fracResult;

        if (a.isNegative) result = "-" + result;
        return result;
    }

    // CASE 2: different signs → subtraction
    int cmp = compareAbsolute(a, b);
    if (cmp == 0) return "0.0";

    bool resultNegative = false;
    const ParsedNumber *larger, *smaller;
    if (cmp > 0) { larger = &a; smaller = &b; resultNegative = a.isNegative; }
    else { larger = &b; smaller = &a; resultNegative = b.isNegative; }

    // Prepare aligned fractions
    string fracL = larger->fracPart, fracS = smaller->fracPart;
    string intL = larger->intPart, intS = smaller->intPart;

    // Subtract fractional part (borrow if needed)
    int borrow = 0;
    string fracRes = "";
    for (int i = (int)fracL.size() - 1; i >= 0; i--) {
        int d1 = fracL[i] - '0' - borrow;
        int d2 = fracS[i] - '0';
        if (d1 < d2) {
            d1 += 10;
            borrow = 1;
        } else {
            borrow = 0;
        }
        fracRes.push_back((d1 - d2) + '0');
    }
    reverse(fracRes.begin(), fracRes.end());

    // Subtract integer part
    if (borrow) {
        // subtract 1 from integer part of larger number
        intL = subtractStrings(intL, "1");
    }
    string intRes = subtractStrings(intL, intS);

    // Remove unnecessary zeros
    while (fracRes.size() > 1 && fracRes.back() == '0')
        fracRes.pop_back();
    while (intRes.size() > 1 && intRes[0] == '0')
        intRes.erase(0, 1);

    string result = intRes;
    if (!fracRes.empty()) result += "." + fracRes;
    if (resultNegative && result != "0") result = "-" + result;
    return result;
}

// ------------------------------------------------------------
// Function: Process a pair of strings and print result
// ------------------------------------------------------------
void processPair(const string& s1, const string& s2) {
    if (!isValidDouble(s1)) {
        cout << s1 << " is not a valid double.\n";
        return;
    }
    if (!isValidDouble(s2)) {
        cout << s2 << " is not a valid double.\n";
        return;
    }

    ParsedNumber n1 = parseNumber(s1);
    ParsedNumber n2 = parseNumber(s2);

    cout << s1 << " + " << s2 << " = " << addDoubles(n1, n2) << "\n";
}

// ------------------------------------------------------------
// Function: Read file and process each line
// ------------------------------------------------------------
void loadNumbersFromFile(const string& filename) {
    ifstream fin(filename);
    if (!fin.is_open()) {
        cerr << "Error: Could not open file.\n";
        return;
    }

    string num1, num2;
    while (fin >> num1 >> num2) {
        processPair(num1, num2);
    }

    fin.close();
}

// ------------------------------------------------------------
// Main
// ------------------------------------------------------------
int main() {
    string filename;
    cout << "Enter filename: ";
    cin >> filename;

    loadNumbersFromFile(filename);
    return 0;
}
