#include "PrepearingConversion.h"

int main(int argc, char** argv)
{
    try {
        prepearing_conversion::PrepearingConversion p1(argc, argv);
        p1.start_conversion();
    }
    catch (const BaseException& err) {
        std::cerr << err.what() << std::endl;
        return err.code();
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 10;
    }
    /*catch (const CannotOpenFile& err) {
        std::cerr << err.what() << std::endl;
        return 1;
    }
    catch (const IncorretWAVFile& err) {
        std::cerr << err.what() << std::endl;
        return 2;
    }
    catch (const NoConvertion& err) {
        std::cerr << err.what() << std::endl;
        return 3;
    }
    catch (const NotEnoughParameters& err) {
        std::cerr << err.what() << std::endl;
        return 4;
    }
    catch (const TooMuchParameters& err) {
        std::cerr << err.what() << std::endl;
        return 5;
    }
    catch (const IncorrectParameters& err) {
        std::cerr << err.what() << std::endl;
        return 6;
    }
    catch (const ReadingError& err) {
        std::cerr << err.what() << std::endl;
        return 7;
    }
    catch (const WritingError& err) {
        std::cerr << err.what() << std::endl;
        return 8;
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        return 9;
    }*/
 
    return 0;
}