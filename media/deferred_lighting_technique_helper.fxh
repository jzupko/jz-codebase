#ifndef TECHNIQUE_NAME
#   error "TECHNIQUE_NAME must be defined."
#endif

technique TECHNIQUE_NAME
{
#if	!defined(DISABLE_MASKING)
    pass Mask
    {
		DISABLE_COLOR_WRITE
		
        AlphaTestEnable = false;
        AlphaBlendEnable = false;
        CullMode = None;
        FillMode = Solid;
        ZEnable = true;
        ZWriteEnable = false;

        StencilRef = 0xFF;
        StencilEnable = true;

        StencilFail = Keep;
        StencilFunc = NotEqual;
        StencilMask = STENCIL_BIT_NO_DEFERRED;
        StencilWriteMask = STENCIL_BIT_DEFERRED_MASK;
#		if BACK_FACE_IS_CCW
			StencilPass = Keep;
			StencilZFail = Replace;
#		else
			StencilPass = Replace;
			StencilZFail = Keep;
#		endif

        TwoSidedStencilMode = true;
        Ccw_StencilFail = Keep;
        Ccw_StencilFunc = NotEqual;
#		if BACK_FACE_IS_CCW
			Ccw_StencilPass = Replace;
			Ccw_StencilZFail = Keep;
#		else
			Ccw_StencilPass = Keep;
			Ccw_StencilZFail = Replace;
#		endif

		VertexShader = compile vs_2_0 VertexSimple();
		PixelShader = compile ps_2_0 FragmentSimple();
    }
#	else
#		undef DISABLE_MASKING
#	endif    

    pass Draw
    {
		JZ_LIT_OUTPUT
		
        AlphaTestEnable = false;
        AlphaBlendEnable = true;
#		if !defined(DRAW_FRONT_FACE)
			CullMode = FRONT_FACE_CULLING;
#		else
			CullMode = BACK_FACE_CULLING;
#			undef DRAW_FRONT_FACE
#		endif
        DestBlend = One;
        FillMode = Solid;
        SrcBlend = One;
        ZEnable = false;
        ZWriteEnable = false;

        StencilRef = 0;
        StencilEnable = true;

        StencilFail = Zero;
        StencilFunc = Equal;
        StencilMask = STENCIL_BIT_DEFERRED_MASK|STENCIL_BIT_NO_DEFERRED;
        StencilPass = Keep;
        StencilWriteMask = STENCIL_BIT_DEFERRED_MASK;

        TwoSidedStencilMode = false;

#		if !defined(VERTEX_SHADER_NAME)
#			define VERTEX_SHADER_NAME VertexDeferredLight
#		endif

#		if !defined(FRAGMENT_SHADER_NAME)
#			define FRAGMENT_SHADER_NAME  JZ_CAT(Fragment, TECHNIQUE_NAME)
#		endif

#		ifdef SHADER_VER
			VertexShader = compile JZ_CAT(vs, SHADER_VER) VERTEX_SHADER_NAME();
			PixelShader = compile JZ_CAT(ps, SHADER_VER) FRAGMENT_SHADER_NAME();
#			undef SHADER_VER
#		else
			VertexShader = compile vs_2_0 VERTEX_SHADER_NAME();
			PixelShader = compile ps_2_0 FRAGMENT_SHADER_NAME();
#		endif
    }

#	undef FRAGMENT_SHADER_NAME
#	undef VERTEX_SHADER_NAME
}

#undef TECHNIQUE_NAME
