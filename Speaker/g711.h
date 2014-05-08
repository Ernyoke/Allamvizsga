#ifndef G711_H
#define G711_H

class G711
{
private:
    static const short SIGN_BIT = 0x80;		/* Sign bit for a A-law byte. */
    static const short QUANT_MASK = 0xf;		/* Quantization field mask. */
    static const short NSEGS = 8;		/* Number of A-law segments. */
    static const short SEG_SHIFT = 4;		/* Left shift for segment number. */
    static const short SEG_MASK = 0x70;		/* Segment field mask. */
    static short seg_aend[8];

    static short search(short, short);

public:
    G711();
    static unsigned char Snack_Lin2Alaw(short);
    static short Snack_Alaw2Lin(unsigned char);
    //static
};

#endif // G711_H
