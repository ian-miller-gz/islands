

#define kLogBandTextureWidth 12

#define TexelLoad2D(x, y) x.Load(int3(y, 0))

Texture2D<float4> Curves : register(t0, space2);
SamplerState CurvesSampler : register(s0, space2);
Texture2D<float4> Bands : register(t1, space2);
SamplerState BandsSampler : register(s1, space2);

uint CalcRootCode(float y1, float y2, float y3)
{

    uint i1 = asuint(y1) >> 31U;
    uint i2 = asuint(y2) >> 30U;
    uint i3 = asuint(y3) >> 29U;

    uint shift = (i2 & 2U) | (i1 & ~2U);
    shift = (i3 & 4U) | (shift & ~4U);

    return ((0x2E74U >> shift) & 0x0101U);
}

float2 SolveHorizPoly(float4 p12, float2 p3)
{

    float2 a = p12.xy - p12.zw * 2.0 + p3;
    float2 b = p12.xy - p12.zw;
    float ra = 1.0 / a.y;
    float rb = 0.5 / b.y;

    float d = sqrt(max(b.y * b.y - a.y * p12.y, 0.0));
    float t1 = (b.y - d) * ra;
    float t2 = (b.y + d) * ra;


    if (abs(a.y) < 1.0 / 65536.0) t1 = t2 = p12.y * rb;


    return (float2((a.x * t1 - b.x * 2.0) * t1 + p12.x,
        (a.x * t2 - b.x * 2.0) * t2 + p12.x));
}

float2 SolveVertPoly(float4 p12, float2 p3)
{

    float2 a = p12.xy - p12.zw * 2.0 + p3;
    float2 b = p12.xy - p12.zw;
    float ra = 1.0 / a.x;
    float rb = 0.5 / b.x;

    float d = sqrt(max(b.x * b.x - a.x * p12.x, 0.0));
    float t1 = (b.x - d) * ra;
    float t2 = (b.x + d) * ra;


    if (abs(a.x) < 1.0 / 65536.0) t1 = t2 = p12.x * rb;


    return (float2((a.y * t1 - b.y * 2.0) * t1 + p12.y,
        (a.y * t2 - b.y * 2.0) * t2 + p12.y));
}

int2 CalcBandLoc(int2 glyphLoc, uint offset)
{

    int2 bandLoc = int2(glyphLoc.x + int(offset), glyphLoc.y);
    bandLoc.y += bandLoc.x >> kLogBandTextureWidth;
    bandLoc.x &= (1 << kLogBandTextureWidth) - 1;
    return (bandLoc);
}

float CalcCoverage(float xcov, float ycov, float xwgt, float ywgt)
{

    float coverage = max(abs(xcov * xwgt + ycov * ywgt) /
            max(xwgt + ywgt, 1.0 / 65536.0),
        min(abs(xcov), abs(ycov)));
    return (saturate(coverage));
}

float SlugRender(Texture2D curveData, Texture2D<float4> bandData, float2 renderCoord, float4 bandTransform, int4 glyphData)
{
    int curveIndex;


    float2 emsPerPixel = fwidth(renderCoord);
    float2 pixelsPerEm = 1.0 / emsPerPixel;

    int2 bandMax = glyphData.zw;
    bandMax.y &= 0x00FF;


    int2 bandIndex = clamp(int2(renderCoord * bandTransform.xy +
        bandTransform.zw), int2(0, 0), bandMax);
    int2 glyphLoc = glyphData.xy;

    float xcov = 0.0;
    float xwgt = 0.0;


    uint2 hbandData = uint2(TexelLoad2D(bandData,
        int2(glyphLoc.x + bandIndex.y, glyphLoc.y)).xy);
    int2 hbandLoc = CalcBandLoc(glyphLoc, hbandData.y);


    for (curveIndex = 0; curveIndex < int(hbandData.x); curveIndex++)
    {

        int2 curveLoc = int2(TexelLoad2D(bandData,
            int2(hbandLoc.x + curveIndex, hbandLoc.y)).xy);


        float4 p12 = TexelLoad2D(curveData, curveLoc) -
            float4(renderCoord, renderCoord);
        float2 p3 = TexelLoad2D(curveData,
            int2(curveLoc.x + 1, curveLoc.y)).xy - renderCoord;


        if (max(max(p12.x, p12.z), p3.x) * pixelsPerEm.x < -0.5) break;

        uint code = CalcRootCode(p12.y, p12.w, p3.y);
        if (code != 0U)
        {

            float2 r = SolveHorizPoly(p12, p3) * pixelsPerEm.x;

            if ((code & 1U) != 0U)
            {
                xcov += saturate(r.x + 0.5);
                xwgt = max(xwgt, saturate(1.0 - abs(r.x) * 2.0));
            }

            if (code > 1U)
            {
                xcov -= saturate(r.y + 0.5);
                xwgt = max(xwgt, saturate(1.0 - abs(r.y) * 2.0));
            }
        }
    }

    float ycov = 0.0;
    float ywgt = 0.0;


    uint2 vbandData = uint2(TexelLoad2D(bandData,
        int2(glyphLoc.x + bandMax.y + 1 + bandIndex.x, glyphLoc.y)).xy);
    int2 vbandLoc = CalcBandLoc(glyphLoc, vbandData.y);


    for (curveIndex = 0; curveIndex < int(vbandData.x); curveIndex++)
    {
        int2 curveLoc = int2(TexelLoad2D(bandData,
            int2(vbandLoc.x + curveIndex, vbandLoc.y)).xy);
        float4 p12 = TexelLoad2D(curveData, curveLoc) -
            float4(renderCoord, renderCoord);
        float2 p3 = TexelLoad2D(curveData,
            int2(curveLoc.x + 1, curveLoc.y)).xy - renderCoord;


        if (max(max(p12.y, p12.w), p3.y) * pixelsPerEm.y < -0.5) break;

        uint code = CalcRootCode(p12.x, p12.z, p3.x);
        if (code != 0U)
        {
            float2 r = SolveVertPoly(p12, p3) * pixelsPerEm.y;

            if ((code & 1U) != 0U)
            {
                ycov -= saturate(r.x + 0.5);
                ywgt = max(ywgt, saturate(1.0 - abs(r.x) * 2.0));
            }

            if (code > 1U)
            {
                ycov += saturate(r.y + 0.5);
                ywgt = max(ywgt, saturate(1.0 - abs(r.y) * 2.0));
            }
        }
    }

    return (CalcCoverage(xcov, ycov, xwgt, ywgt));
}

struct Input
{
    float4 Position : SV_Position;
    float4 Color : TEXCOORD0;
    float2 Coordinates : TEXCOORD1;
    nointerpolation float4 Banding : TEXCOORD2;
    nointerpolation int4 Glyph : TEXCOORD3;
};

float4 main(Input input) : SV_Target0
{
    float coverage = SlugRender(Curves, Bands, input.Coordinates,
        input.Banding, input.Glyph);
    return (input.Color * coverage);
}
