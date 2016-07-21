//
//  DepthSmoothShader.cpp
//
//  Created by James Alliban's MBP on 30/04/2014.
//
//

#include "DepthSmoothShader.hpp"



void DepthSmoothShader::init()
{
    string fragShaderStr = STRINGIFY(
    
                                     );
    
    setupShaderFromSource(GL_FRAGMENT_SHADER, fragShaderStr);
    linkProgram();
}