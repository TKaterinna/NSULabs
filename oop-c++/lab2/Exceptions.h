#pragma once
#include <exception>
#include <string>

class BaseException
{
private:
    std::string message;
    int code_exc = 1;
public:
    BaseException() : message{ "Error" }, code_exc{ 1 } {};
    BaseException(const std::string& message, int code_exc) : message{ message }, code_exc{ code_exc } {};
    virtual ~BaseException() {};
    const std::string what() const noexcept {
        return message;
    }
    int code() const noexcept {
        return code_exc;
    }
};

class CannotOpenFile : public BaseException {
private:
    std::string message;
    int code_exc;
public:
    CannotOpenFile(const std::string& message) : BaseException("Cannot open file " + message, 2) {};
    //const char* what() const noexcept override
    //{
    //    return message.c_str();     // получаем из std::string строку const char*
    //}
};

class IncorretWAVFile : public BaseException {
private:
    std::string message;
    int code_exc;
public:
    IncorretWAVFile(const std::string& message) : BaseException("Inncorect type of WAV file " + message, 3) {};
    /*const char* what() const noexcept override
    {
        return message.c_str();
    }*/
};

class NoConvertion : public BaseException {
private:
    std::string message;
    int code_exc;
public:
    NoConvertion(const std::string& message) : BaseException("Convertion " + message + " does not exists", 4) {};
    /*const char* what() const noexcept override
    {
        return message.c_str();
    }*/
};

class NotEnoughParameters : public BaseException {
private:
    std::string message;
    int code_exc;
public:
    NotEnoughParameters(const std::string& message) : BaseException("Not enough parameters for " + message, 5) {};
    /*const char* what() const noexcept override
    {
        return message.c_str();
    }*/
};

class TooMuchParameters : public BaseException {
private:
    std::string message;
    int code_exc;
public:
    TooMuchParameters(const std::string& message) : BaseException("Too much parameters for " + message, 6) {};
    /*const char* what() const noexcept override
    {
        return message.c_str();
    }*/
};

class IncorrectParameters : public BaseException {
private:
    std::string message;
    int code_exc;
public:
    IncorrectParameters(const std::string& message) : BaseException("Incorrect parameters: " + message, 7) {};
    /*const char* what() const noexcept override
    {
        return message.c_str();
    }*/
};

class ReadingError : public BaseException {
private:
    std::string message;
    int code_exc;
public:
    ReadingError() : message{ "Error while reading" }, code_exc{ 8 } {};
    ReadingError(const std::string& message) : BaseException("Error while reading: " + message, 8) {};
    /*const char* what() const noexcept override
    {
        return message.c_str();
    }*/
};

class WritingError : public BaseException {
private:
    std::string message;
    int code_exc;
public:
    WritingError() : message{ "Error while writing" }, code_exc{ 9 } {};
    WritingError(const std::string& message) : BaseException("Error while writing: " + message, 9) {};
    int code() const noexcept {
        return code_exc;
    }
    /*const char* what() const noexcept override
    {
        return message.c_str();
    }*/
};


//class CannotOpenFile : public std::exception {
//private:
//    std::string message;
//public:
//    CannotOpenFile(const std::string& message) : message{ "Cannot open file " + message } {};
//    const char* what() const noexcept override
//    {
//        return message.c_str();     // получаем из std::string строку const char*
//    }
//};
//
//class IncorretWAVFile : public std::exception {
//private:
//    std::string message;
//public:
//    IncorretWAVFile(const std::string& message) : message{ "Inncorect type of WAV file " + message } {};
//    const char* what() const noexcept override
//    {
//        return message.c_str();
//    }
//};
//
//class NoConvertion : public std::exception {
//private:
//    std::string message;
//public:
//    NoConvertion(const std::string& message) : message{ "Convertion " + message + " does not exists" } {};
//    const char* what() const noexcept override
//    {
//        return message.c_str();
//    }
//};
//
//class NotEnoughParameters : public std::exception {
//private:
//    std::string message;
//public:
//    NotEnoughParameters(const std::string& message) : message{ "Not enough parameters for " + message } {};
//    const char* what() const noexcept override
//    {
//        return message.c_str();
//    }
//};
//
//class TooMuchParameters : public std::exception {
//private:
//    std::string message;
//public:
//    TooMuchParameters(const std::string& message) : message{ "Too much parameters for " + message } {};
//    const char* what() const noexcept override
//    {
//        return message.c_str();
//    }
//};
//
//class IncorrectParameters : public std::exception {
//private:
//    std::string message;
//public:
//    IncorrectParameters(const std::string& message) : message{ "Incorrect parameters: " + message } {};
//    const char* what() const noexcept override
//    {
//        return message.c_str();
//    }
//};
//
//class ReadingError : public std::exception {
//private:
//    std::string message;
//public:
//    ReadingError() : message{ "Error while reading" } {};
//    ReadingError(const std::string& message) : message{ "Error while reading: " + message } {};
//    const char* what() const noexcept override
//    {
//        return message.c_str();
//    }
//};
//
//class WritingError : public std::exception {
//private:
//    std::string message;
//public:
//    WritingError() : message{ "Error while writing" } {};
//    WritingError(const std::string& message) : message{ "Error while writing: " + message } {};
//    const char* what() const noexcept override
//    {
//        return message.c_str();
//    }
//};