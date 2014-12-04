/*!
 * \file rShader.cpp
 * \brief \b Classes: \a rShader
 * \sa rShader
 */
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <GL/glew.h>
#include "rShader.hpp"
#include <boost/filesystem.hpp>
#include "uLog.hpp"
#include "defines.hpp"
#include "eCMDColor.hpp"
#include <stdio.h>


namespace e_engine {

rShader::rShader() {
   vShaderProgram_OGL    = 0;
   vIsShaderLinked_B     = false;

   vShaderEndings[VERT]  = VERT_END;
   vShaderEndings[FRAG]  = FRAG_END;
   vShaderEndings[GEOM]  = GEOM_END;

   for( size_t i = 0; i < __END_INF__; ++i )
      vLocationInformation[i] = -1;

   // Inputs:

   vLocationNames[VERTEX_INPUT]       = "iVertex";
   vLocationTypes[VERTEX_INPUT]       = GL_FLOAT_VEC3;

   vLocationNames[NORMALS_INPUT]      = "iNormals";
   vLocationTypes[NORMALS_INPUT]      = GL_FLOAT_VEC3;

   // Uniforms:

   vLocationNames[MODEL_MATRIX]       = "uModel";
   vLocationTypes[MODEL_MATRIX]       = GL_FLOAT_MAT4;

   vLocationNames[VIEW_MATRIX]        = "uView";
   vLocationTypes[VIEW_MATRIX]        = GL_FLOAT_MAT4;

   vLocationNames[PROJECTOIN_MATRIX]  = "uProjection";
   vLocationTypes[PROJECTOIN_MATRIX]  = GL_FLOAT_MAT4;

   vLocationNames[M_V_P_MATRIX]       = "uMVP";
   vLocationTypes[M_V_P_MATRIX]       = GL_FLOAT_MAT4;


   vLocationNames[AMBIENT_COLOR]      = "uAmbientColor";
   vLocationTypes[AMBIENT_COLOR]      = GL_FLOAT_VEC3;

   vLocationNames[LIGHT_COLOR]        = "uLightColor";
   vLocationTypes[LIGHT_COLOR]        = GL_FLOAT_VEC3;

   vLocationNames[LIGHT_POSITION]     = "uLightPos";
   vLocationTypes[LIGHT_POSITION]     = GL_FLOAT_VEC3;
}

rShader::rShader( rShader && _s ) :
   vShaders( std::move( _s.vShaders ) ),
   vPath_str( std::move( _s.vPath_str ) ),
   vShaderProgram_OGL( std::move( _s.vShaderProgram_OGL ) ),
   vIsShaderLinked_B( std::move( _s.vIsShaderLinked_B ) ),
   vProgramInformation( std::move( _s.vProgramInformation ) ),
   vHasProgramInformation_B( std::move( _s.vHasProgramInformation_B ) ) {

   for( unsigned int i = 0; i < 3; ++i )
      vShaderEndings[i] = std::move( _s.vShaderEndings[i] );

   for( unsigned int i = 0; i < __END_INF__; ++i )
      vLocationInformation[i] = std::move( _s.vLocationInformation[i] );

   for( unsigned int i = 0; i < __END_INF__; ++i )
      vLocationNames[i] = std::move( _s.vLocationNames[i] );

   for( unsigned int i = 0; i < __END_INF__; ++i )
      vLocationTypes[i] = std::move( _s.vLocationTypes[i] );

}


// =========================================================================================================================
// ==============================================================================================================================================
// =========                =================================================================================================================================
// =======   Link the shader  ====================================================================================================================================
// =========                =================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

/*!
 * \brief Search for shader files
 * \returns The number of found shader files
 */
int rShader::search_shaders() {
   std::string temp[] = {vPath_str, vPath_str, vPath_str};

   temp[VERT] += vShaderEndings[VERT];
   temp[FRAG] += vShaderEndings[FRAG];
   temp[GEOM] += vShaderEndings[GEOM];

   boost::filesystem::path vert( temp[VERT] );
   boost::filesystem::path frag( temp[FRAG] );
   boost::filesystem::path geom( temp[GEOM] );

   try {
      if( boost::filesystem::exists( vert ) )     // Look for the vertex-shader
         if( boost::filesystem::is_regular_file( vert ) )
            vShaders.emplace_back( temp[VERT], GL_VERTEX_SHADER );

      if( boost::filesystem::exists( frag ) )     // Look for the fragment-shader
         if( boost::filesystem::is_regular_file( vert ) )
            vShaders.emplace_back( temp[FRAG], GL_FRAGMENT_SHADER );

      if( boost::filesystem::exists( geom ) )     // Look for the geometry-shader
         if( boost::filesystem::is_regular_file( geom ) )
            vShaders.emplace_back( temp[GEOM], GL_GEOMETRY_SHADER );


   } catch( const boost::filesystem::filesystem_error &ex ) {
      eLOG( ex.what() );
   } catch( ... ) {
      eLOG( "Caught unknown exception" );
   }

   if( vShaders.size() == 0 ) {
      wLOG( "No shaders found! WARNING!" );
   }

   return vShaders.size();
}


bool rShader::addShader( std::string _filename, GLenum _shaderType ) {
   boost::filesystem::path lShaderPath( _filename );

   try {
      if( boost::filesystem::exists( lShaderPath ) ) {     // Look for the vertex-shader
         if( boost::filesystem::is_regular_file( lShaderPath ) ) {

            switch( _shaderType ) {
               case GL_VERTEX_SHADER:   vShaders.emplace_back( _filename, GL_VERTEX_SHADER );   return true;
               case GL_FRAGMENT_SHADER: vShaders.emplace_back( _filename, GL_FRAGMENT_SHADER ); return true;
               case GL_GEOMETRY_SHADER: vShaders.emplace_back( _filename, GL_GEOMETRY_SHADER ); return true;
               default:
                  eLOG( "Unknown shader type ", _shaderType, " of ", _filename );
                  return false;
            }
         }
      }
   } catch( const boost::filesystem::filesystem_error &ex ) {
      eLOG( ex.what() );
   } catch( ... ) {
      eLOG( "Caught unknown exception" );
   }

   eLOG( "Failed to access shader file ", _filename );
   return false;
}


/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */
/*!
 * \brief Test if program is OK
 * \returns 1 When successful
 * \returns -5 When a shader linking error occured
 */
unsigned int rShader::testProgram() {
   int status;
   glGetProgramiv( vShaderProgram_OGL, GL_LINK_STATUS, &status );
   if( status == GL_FALSE ) {
      GLint lLinkLogLength;
      glGetProgramiv( vShaderProgram_OGL, GL_INFO_LOG_LENGTH, &lLinkLogLength );
      GLchar *log = new GLchar[lLinkLogLength];
      glGetProgramInfoLog( vShaderProgram_OGL, lLinkLogLength, NULL, log );

      eLOG(
            "Linking failure:\n"
            "###################################################################################\n\n",
            std::string( log ),
            "\n\n###################################################################################"

      );
      glDeleteProgram( vShaderProgram_OGL );

      vIsShaderLinked_B = false;

      delete[] log;

      //Return a shader linking error
      return -5;
   }
   return 1;
}


/*!
 *
 * \brief Search the shader files, compile them and link the program
 *
 * \returns the number of the (successfully) compiled and linked shader files
 * \returns -2 When a file reading error occurs
 * \returns -3 When a file not found error occurs
 * \returns -4 When a shader compilation error occurs
 * \returns -5 When a shader linking error occurs
 * \returns -6 When the shaders were not set
 */
int rShader::compile( GLuint &_vShader_OGL ) {
   unsigned int lRet_uI = compile();
   _vShader_OGL = vShaderProgram_OGL;
   return lRet_uI;
}

/*!
 *
 * \brief Search the shader files, compile them and link the program
 *
 * \returns the number of the (successfully) compiled and linked shader files
 * \returns -2 When a file reading error occurs
 * \returns -3 When a file not found error occurs
 * \returns -4 When a shader compilation error occurs
 * \returns -5 When a shader linking error occurs
 * \returns -6 When the shaders were not set
 */
int rShader::compile() {
   if( vPath_str.empty() && vShaders.empty() ) {
      eLOG( "No shaders set for compilation" );
      return -6;
   }

   if( search_shaders() == 0  && vShaders.empty() ) {
      eLOG( "Unable to find any shader file ( Path: ", vPath_str, " )" );
      //Return a file not found error
      return -3;
   }

   for( auto & s : vShaders ) {
      // Compile shaders
      //Check for errors
      if( s.compileShader() != 1 )
         return -4;
   }


   // Createng the program
   vShaderProgram_OGL = glCreateProgram();

   // Adding shaders
   for( auto & s : vShaders ) {
      glAttachShader( vShaderProgram_OGL, s.vShader_OGL );
   }

   // Linking
   glLinkProgram( vShaderProgram_OGL );

   // Delete old shaders. Not needed anymore
   for( auto & s : vShaders ) {
      glDeleteShader( s.vShader_OGL );
   }

   // Linking successful? Returns a shader linking error if unsuccessful
   if( testProgram() != 1 )
      return -5;


   // Output
   iLOG( "Program with the ", vShaders.size(), " shader(s) successfully linked" );
   for( auto & s : vShaders ) {
      LOG( _hI, eCMDColor::color( 'O', 'C' ), "  - ", s.vFilename_str );
   }
   vIsShaderLinked_B = true;

   int lTempShaderCounter_I = vShaders.size();
   vShaders.clear();
   getProgramInfo();

   return lTempShaderCounter_I;
}


/*!
 * \brief Deletes the programm
 * \returns Nothing
 */
void rShader::deleteProgram() {
   if( !vIsShaderLinked_B )
      return; // Don't delete twice
   glDeleteProgram( vShaderProgram_OGL );
   vIsShaderLinked_B = false;
}

// =========================================================================================================================
// ==============================================================================================================================================
// =========                    =============================================================================================================================
// =======   Compile the Shaders  ================================================================================================================================
// =========                    =============================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

std::string getShaderTypeString( GLenum _type ) {
   switch( _type ) {
      case GL_VERTEX_SHADER_ARB:   return "vertex";
      case GL_FRAGMENT_SHADER_ARB: return "fragment";
      case GL_GEOMETRY_SHADER_ARB: return "geometry";
      default:                     return "unknown";
   }
}

bool rShader::singleShader::readShader() {
   FILE *lFile = fopen( vFilename_str.c_str(), "r" );
   if( lFile == NULL ) {
      eLOG( "Unable to open ", vFilename_str );
      return false;
   }

   int c;
   vData_str.clear();

   while( ( c = fgetc( lFile ) ) != EOF )
      vData_str.append( 1, ( char )c );

   fclose( lFile );

   return true;
}

/*!
 * Checks for compiler errors
 *
 * \returns true  When successful
 * \returns false When a shader compilation error occurs
 */
bool rShader::singleShader::testShader() {
   int test;
   glGetShaderiv( vShader_OGL, GL_COMPILE_STATUS, &test );
   if( test == GL_FALSE ) {
      GLint logLength = 0;
      glGetShaderiv( vShader_OGL, GL_INFO_LOG_LENGTH, &logLength );
      GLchar *log = new GLchar[logLength];
      glGetShaderInfoLog( vShader_OGL, logLength, NULL, log );

      eLOG(
            "Compile failure in the ", getShaderTypeString( vShaderType ), " shader ", vFilename_str, ":\n",
            "###################################################################################\n\n",
            std::string( log ),
            "\n\n###################################################################################"
      );
      glDeleteShader( vShader_OGL );

      delete[] log;

      //Returns a shader compilation error
      return false;
   }
   return true;
}

/*!
 * This runs read_shader, compiles
 * the shader and returns it
 *
 * \param _shader The shader that will be created
 * \returns 1 When successful
 * \returns 2 When a file-reading error occurs
 * \returns 3 When a compile error occurs
 */
int rShader::singleShader::compileShader() {
   vData_str.clear();
   if( readShader() == false ) {
      eLOG( "Error while reading source file '", vFilename_str, "'" );
      //Return the file-reading error
      return 2;
   }

   const GLcharARB *temp = vData_str.c_str();
   // Create Shader
   vShader_OGL = glCreateShader( vShaderType );
   // Adding source
   glShaderSource( vShader_OGL, 1, &temp, NULL );
   // Compiling
   glCompileShader( vShader_OGL );
   vData_str.clear();


   return testShader() == true ? 1 : 3;
}







/*!
 * \brief Get the linked program
 * \param[out] _program The linked shader program
 * \returns true if the program was linked and false if not
 */
bool rShader::getProgram( unsigned int &_program ) const  {
   if( !vIsShaderLinked_B )
      return false;
   _program = vShaderProgram_OGL;
   return true;
}



/* ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** **
 ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** ** */


void rShader::setLocationString( SHADER_INFORMATION _type, std::string &_str ) {
   vLocationNames[_type] = _str;
}

void rShader::setLocationString( SHADER_INFORMATION _type, std::string && _str ) {
   vLocationNames[_type] = _str;
}


/*!
 * \brief Interpretes the information in the shader info struct
 *
 * This function Interpretes the information in the shader info struct and assigns
 * the location of the input / uniform to a specific type.
 *
 * Defult strings:
 *
 * |    name     |        type       |
 * | :---------: | :---------------: |
 * | iVertex     | VERTEX_INPUT      |
 * | iNormals    | NORMALS_INPUT     |
 * | uModel      | MODEL_MATRIX      |
 * | uView       | VIEW_MATRIX       |
 * | uProjection | PROJECTOIN_MATRIX |
 * | uMVP        | M_V_P_MATRIX      |
 *
 * \returns true if everything went fine and false when at least one value could not be assigned
 */
bool rShader::parseRawInformation() {
   if( !vIsShaderLinked_B )
      return false;

#if E_DEBUG_LOGGING
   dLOG( "Assigning locations of shader '", vPath_str, "' to a type" );
#endif

   GLint j;
   bool lRet = true;

   for( auto const & i : vProgramInformation.vInputInfo ) {
      for( j = 0; j < __BEGIN_UNIFORMS__; ++j ) {
         if( vLocationInformation[j] >= 0 )
            continue;

         if( i.name == vLocationNames[j] && i.type == vLocationTypes[j] ) {
            vLocationInformation[j] = i.location;
            j = -1;
            break;
         }
      }

      if( j >= 0 ) {
         wLOG( "  - Failed to assign input '", i.name, "' [", i.location, "; ", getTypeString( i.type ), "]" );
         lRet = false;
      }
   }

   for( auto const & i : vProgramInformation.vUniformInfo ) {
      for( j = __BEGIN_UNIFORMS__ + 1; j < __END_INF__; ++j ) {
         if( vLocationInformation[j] >= 0 )
            continue;

         if( i.name == vLocationNames[j] && i.type == vLocationTypes[j] ) {
            vLocationInformation[j] = i.location;
            j = -1;
            break;
         }
      }

      if( j >= 0 ) {
         wLOG( "  - Failed to assign uniform '", i.name, "' [", i.location, "; ", getTypeString( i.type ), "]" );
         lRet = false;
      }
   }

   return lRet;
}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;

