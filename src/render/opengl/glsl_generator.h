//
//  glsl_generator.h
//  GHL
//
//  Created by Andrey Kunitsyn on 11/18/12.
//  Copyright (c) 2012 AndryBlack. All rights reserved.
//

#ifndef __GHL__glsl_generator__
#define __GHL__glsl_generator__

#include "../pfpl/pfpl_cache.h"
#include "ghl_render.h"

namespace GHL {
    
    class GLSLGenerator : public pfpl_shader_generator_base {
    public:
        GLSLGenerator();
        void init(GHL::Render* render);
        virtual ShaderProgram* generate( const pfpl_state_data& entry, bool tex2 );
    private:
        GHL::Render*    m_render;
        VertexShader* m_simple_v;
    };
    
}
#endif /* defined(__GHL__glsl_generator__) */
