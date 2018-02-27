#ifndef __WAVEDROM_H__
#define __WAVEDROM_H__

#include <inttypes.h>
#include <sstream>
#include <vector>
#include <map>

class WaveDromSignal {
    std::string mName;
    std::string mValues;
    bool mIsNumeric;
    float mPhase;
    std::vector<std::string> mData;
    bool mHasPreviousValue;
    char mPreviousValue;
    std::string mPreviousStringValue;

public:
    WaveDromSignal(std::string const &name, bool isNumeric=false) :
        mName(name),
        mIsNumeric(isNumeric),
        mHasPreviousValue(false),
        mPhase(0.0f)
    {
        // Nothing.
    }

    WaveDromSignal &setPhase(float phase) {
        mPhase = phase;
        return *this;
    }

    std::string const &name() const {
        return mName;
    }

    void add(unsigned long value) {
        char ch;

        if (mName == "clock") {
            ch = 'P';
        } else if (mName == "full_clock") {
            ch = value ? 'H' : 'l';
        } else if (mIsNumeric) {
            ch = '=';
        } else if (value) {
            ch = '1';
        } else {
            ch = '0';
        }

        if (mHasPreviousValue && value == mPreviousValue) {
            ch = '.';
        } else if (mIsNumeric) {
            mData.push_back(std::to_string(value));
        }

        mValues += ch;

        mHasPreviousValue = true;
        mPreviousValue = value;
    }

    void add(std::string const &value) {
        char ch = '=';

        if (mHasPreviousValue && value == mPreviousStringValue) {
            ch = '.';
        } else {
            mData.push_back(value);
        }

        mValues += ch;

        mHasPreviousValue = true;
        mPreviousStringValue = value;
    }

    void write(std::ofstream &f) const {
        f << "    { \"name\": \"" << mName << "\", \"wave\": \"" << mValues << "\"";

        if (!mData.empty()) {
            f << ", \"data\": [";
            for (int i = 0; i < mData.size(); i++) {
                f << "\"" << mData[i] << "\"";
                if (i < mData.size() - 1) {
                    f << ", ";
                }
            }
            f << "]";
        }

        if (mPhase != 0.0f) {
            f << ", \"phase\": " << mPhase;
        }
   
        f << "}";
    }
};

class WaveDrom {
public:
    std::vector<WaveDromSignal> mSignals;
    std::map<std::string,int> mNameToIndex;

    void add(WaveDromSignal const &signal) {
        // Remember its index by name.
        mNameToIndex[signal.name()] = mSignals.size();

        mSignals.push_back(signal);
    }

    WaveDromSignal &operator[](std::string const &name) {
        // Use at() so that it throws an exception if the name isn't found.
        return mSignals[mNameToIndex.at(name)];
    }

    void write(std::string const &pathname) {
        std::ofstream f;

        f.open(pathname, std::ios::out);
        f << "{\"head\": {" << std::endl;
        f << "    \"tick\": 0" << std::endl;
        f << "}, \"signal\": [" << std::endl;

        for (int i = 0; i < mSignals.size(); i++) {
            mSignals[i].write(f);
            if (i < mSignals.size() - 1) {
                f << ",";
            }
            f << std::endl;
        }
        f << "]}" << std::endl;
        f.close();
    }
};

// Convert the value to a hex string of "digits" digits.
std::string to_hex(uint64_t value, int digits) {
    std::stringstream ss;

    ss.width(digits);
    ss.fill('0');

    ss << std::hex << std::uppercase << value;

    return ss.str();
}

#endif // __WAVEDROM_H__
