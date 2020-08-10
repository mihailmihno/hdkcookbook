#ifndef __SOP_HDKCookDemo00_h__
#define __SOP_HDKCookDemo00_h__

#include <SOP/SOP_Node.h>

namespace HDK_CookDemo
{
    class SOP_HDKCookDemo00 : public SOP_Node
    {
    public:
        static PRM_Template parmTemplateList[];
        static OP_Node *myConstructor(OP_Network *net, const char *name, OP_Operator *op)
        {
            return new SOP_HDKCookDemo00(net, name, op);
        }

        static const UT_StringHolder theSOPTypeName;

    protected:
        SOP_HDKCookDemo00(OP_Network* net, const char* name, OP_Operator* op) : SOP_Node(net, name, op)
        {
            mySopFlags.setManagesDataIDs(true);
        }

        virtual ~SOP_HDKCookDemo00() {}
        virtual OP_ERROR cookMySop(OP_Context& context) override
        {
            return cookDemo(context);
        }
    private:
        OP_ERROR cookDemo(OP_Context& context);
        OP_ERROR cookPyramidPoints(GU_Detail* gdp, fpreal t, int& offset);
        OP_ERROR cookBoxPoints(GU_Detail* gdp, fpreal t, int& offset);
        OP_ERROR cookDiamondPoints(GU_Detail* gdp, fpreal t, int& offset);
        //
        OP_ERROR addDetail(GU_Detail* gdp, UT_Vector3 p0, UT_Vector3 p1, int& offset, int detail);
        //
        int USEPOS(fpreal t) { return evalInt("usetgt", 0, t); }
        int SHAPE(fpreal t) { return evalInt("shape", 0, t); }
        float DEPTHSCALE(fpreal t) { return evalFloat("depthscale", 0, t); }
        int DEPTH(fpreal t) { return evalInt("depth", 0, t); }
        int DETAIL(fpreal t) { return evalInt("detail", 0, t); }
        fpreal SCALEX(fpreal t) { return evalFloat("scale", 0, t); }
        fpreal SCALEY(fpreal t) { return evalFloat("scale", 1, t); }
        fpreal SCALEZ(fpreal t) { return evalFloat("scale", 2, t); }
        void TARGET(fpreal32* out, fpreal t) { return evalFloats("tgtpos", out, t); }
    };
}
#endif
