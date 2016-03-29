/*!
 * \file @FILENAME_CPP@
 * \brief \b Classes: \a @CLASSNAME@
 * \warning This file was automatically generated by createSPIRV!
 */

#include "@FILENAME_HPP@"

namespace e_engine {

const std::vector<unsigned char> @CLASSNAME@::vRawData_vert = {
@FINAL_DATA_VERT@
};

const std::vector<unsigned char> @CLASSNAME@::vRawData_tesc = {
@FINAL_DATA_TESC@
};

const std::vector<unsigned char> @CLASSNAME@::vRawData_tese = {
@FINAL_DATA_TESE@
};

const std::vector<unsigned char> @CLASSNAME@::vRawData_geom = {
@FINAL_DATA_GEOM@
};

const std::vector<unsigned char> @CLASSNAME@::vRawData_frag = {
@FINAL_DATA_FRAG@
};

const std::vector<unsigned char> @CLASSNAME@::vRawData_comp = {
@FINAL_DATA_COMP@
};

std::vector<unsigned char> @CLASSNAME@::getRawData_vert() const { return vRawData_vert; }
std::vector<unsigned char> @CLASSNAME@::getRawData_tesc() const { return vRawData_tesc; }
std::vector<unsigned char> @CLASSNAME@::getRawData_tese() const { return vRawData_tese; }
std::vector<unsigned char> @CLASSNAME@::getRawData_geom() const { return vRawData_geom; }
std::vector<unsigned char> @CLASSNAME@::getRawData_frag() const { return vRawData_frag; }
std::vector<unsigned char> @CLASSNAME@::getRawData_comp() const { return vRawData_comp; }

}
