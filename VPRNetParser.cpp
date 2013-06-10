
#line 1 "VPRNetParser.rl"
#include "VPRNetParser.hpp"


#line 99 "VPRNetParser.rl"



/* Regal data ****************************************/

#line 13 "VPRNetParser.cpp"
static const int VPRNetParser_start = 1;
static const int VPRNetParser_error = 0;

static const int VPRNetParser_en_main = 1;


#line 104 "VPRNetParser.rl"
/* Regal data: end ***********************************/


void VPRNetParser::init() {
    buf = &buf_vector[0];
    BUFSIZE = buf_vector.size();

	
#line 29 "VPRNetParser.cpp"
	{
	cs = VPRNetParser_start;
	}

#line 112 "VPRNetParser.rl"
}


void VPRNetParser::parse(std::istream &in_stream) {
    bool done = false;
    int i = 0;
    have = 0;
    while ( !done ) {
        /* How much space is in the buffer? */
        int space = BUFSIZE - have;
        if ( space == 0 ) {
            /* Buffer is full. */
            cerr << "TOKEN TOO BIG" << endl;
            exit(1);
        }
        /* Read in a block after any data we already have. */
        char *p = buf + have;
        in_stream.read( p, space );
        int len = in_stream.gcount();
        char *pe = p + len;
        char *eof = 0;

        /* If no data was read indicate EOF. */
        if ( len == 0 ) {
            eof = pe;
            done = true;
        } else {
            
#line 63 "VPRNetParser.cpp"
	{
	if ( p == pe )
		goto _test_eof;
	switch ( cs )
	{
case 1:
	switch( (*p) ) {
		case 9: goto st2;
		case 10: goto st131;
		case 32: goto st2;
		case 35: goto st3;
		case 46: goto tr4;
	}
	goto st0;
st0:
cs = 0;
	goto _out;
tr173:
#line 27 "VPRNetParser.rl"
	{
        be = p;
        te = p;
	}
	goto st2;
tr181:
#line 11 "VPRNetParser.rl"
	{
        process_input();
	}
	goto st2;
tr185:
#line 15 "VPRNetParser.rl"
	{
        process_output();
	}
	goto st2;
st2:
	if ( ++p == pe )
		goto _test_eof2;
case 2:
#line 104 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st2;
		case 10: goto st131;
		case 32: goto st2;
		case 35: goto st3;
	}
	goto st0;
tr174:
#line 27 "VPRNetParser.rl"
	{
        be = p;
        te = p;
	}
	goto st131;
tr178:
#line 19 "VPRNetParser.rl"
	{
        if(ts != be) {
            ls = buf;
        }
        ts = 0;
        process_clb();
	}
	goto st131;
tr182:
#line 11 "VPRNetParser.rl"
	{
        process_input();
	}
	goto st131;
tr186:
#line 15 "VPRNetParser.rl"
	{
        process_output();
	}
	goto st131;
st131:
	if ( ++p == pe )
		goto _test_eof131;
case 131:
#line 145 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr173;
		case 10: goto tr174;
		case 32: goto tr173;
		case 35: goto tr175;
		case 46: goto tr176;
	}
	goto st0;
tr175:
#line 27 "VPRNetParser.rl"
	{
        be = p;
        te = p;
	}
	goto st3;
tr179:
#line 19 "VPRNetParser.rl"
	{
        if(ts != be) {
            ls = buf;
        }
        ts = 0;
        process_clb();
	}
	goto st3;
tr183:
#line 11 "VPRNetParser.rl"
	{
        process_input();
	}
	goto st3;
tr187:
#line 15 "VPRNetParser.rl"
	{
        process_output();
	}
	goto st3;
st3:
	if ( ++p == pe )
		goto _test_eof3;
case 3:
#line 187 "VPRNetParser.cpp"
	if ( (*p) == 10 )
		goto st131;
	goto st3;
tr4:
#line 6 "VPRNetParser.rl"
	{
		ls = p;
		ts = p;
	}
	goto st4;
tr176:
#line 27 "VPRNetParser.rl"
	{
        be = p;
        te = p;
	}
#line 6 "VPRNetParser.rl"
	{
		ls = p;
		ts = p;
	}
	goto st4;
tr180:
#line 19 "VPRNetParser.rl"
	{
        if(ts != be) {
            ls = buf;
        }
        ts = 0;
        process_clb();
	}
#line 6 "VPRNetParser.rl"
	{
		ls = p;
		ts = p;
	}
	goto st4;
tr184:
#line 11 "VPRNetParser.rl"
	{
        process_input();
	}
#line 6 "VPRNetParser.rl"
	{
		ls = p;
		ts = p;
	}
	goto st4;
tr188:
#line 15 "VPRNetParser.rl"
	{
        process_output();
	}
#line 6 "VPRNetParser.rl"
	{
		ls = p;
		ts = p;
	}
	goto st4;
st4:
	if ( ++p == pe )
		goto _test_eof4;
case 4:
#line 251 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 99: goto st5;
		case 103: goto st58;
		case 105: goto st68;
		case 111: goto st99;
	}
	goto st0;
st5:
	if ( ++p == pe )
		goto _test_eof5;
case 5:
	if ( (*p) == 108 )
		goto st6;
	goto st0;
st6:
	if ( ++p == pe )
		goto _test_eof6;
case 6:
	if ( (*p) == 98 )
		goto st7;
	goto st0;
st7:
	if ( ++p == pe )
		goto _test_eof7;
case 7:
	switch( (*p) ) {
		case 9: goto st8;
		case 32: goto st8;
	}
	goto st0;
st8:
	if ( ++p == pe )
		goto _test_eof8;
case 8:
	switch( (*p) ) {
		case 9: goto st8;
		case 32: goto st8;
		case 95: goto tr12;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr12;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr12;
	} else
		goto tr12;
	goto st0;
tr12:
#line 53 "VPRNetParser.rl"
	{
        label_start = p;
    }
	goto st9;
st9:
	if ( ++p == pe )
		goto _test_eof9;
case 9:
#line 310 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr13;
		case 10: goto tr14;
		case 32: goto tr13;
		case 35: goto tr15;
		case 95: goto st9;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st9;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st9;
	} else
		goto st9;
	goto st0;
tr13:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st10;
st10:
	if ( ++p == pe )
		goto _test_eof10;
case 10:
#line 341 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st10;
		case 10: goto st11;
		case 32: goto st10;
		case 35: goto st57;
	}
	goto st0;
tr14:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st11;
st11:
	if ( ++p == pe )
		goto _test_eof11;
case 11:
#line 363 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st11;
		case 32: goto st11;
		case 112: goto tr20;
	}
	goto st0;
tr20:
#line 32 "VPRNetParser.rl"
	{
        pin_list.clear();
        in_pin_list = true;
    }
	goto st12;
st12:
	if ( ++p == pe )
		goto _test_eof12;
case 12:
#line 381 "VPRNetParser.cpp"
	if ( (*p) == 105 )
		goto st13;
	goto st0;
st13:
	if ( ++p == pe )
		goto _test_eof13;
case 13:
	if ( (*p) == 110 )
		goto st14;
	goto st0;
st14:
	if ( ++p == pe )
		goto _test_eof14;
case 14:
	if ( (*p) == 108 )
		goto st15;
	goto st0;
st15:
	if ( ++p == pe )
		goto _test_eof15;
case 15:
	if ( (*p) == 105 )
		goto st16;
	goto st0;
st16:
	if ( ++p == pe )
		goto _test_eof16;
case 16:
	if ( (*p) == 115 )
		goto st17;
	goto st0;
st17:
	if ( ++p == pe )
		goto _test_eof17;
case 17:
	if ( (*p) == 116 )
		goto st18;
	goto st0;
st18:
	if ( ++p == pe )
		goto _test_eof18;
case 18:
	if ( (*p) == 58 )
		goto st19;
	goto st0;
st19:
	if ( ++p == pe )
		goto _test_eof19;
case 19:
	switch( (*p) ) {
		case 9: goto st20;
		case 32: goto st20;
	}
	goto st0;
st20:
	if ( ++p == pe )
		goto _test_eof20;
case 20:
	switch( (*p) ) {
		case 9: goto st20;
		case 32: goto st20;
		case 95: goto tr29;
		case 111: goto tr30;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr29;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr29;
	} else
		goto tr29;
	goto st0;
tr29:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st21;
st21:
	if ( ++p == pe )
		goto _test_eof21;
case 21:
#line 465 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr31;
		case 10: goto tr32;
		case 32: goto tr31;
		case 35: goto tr33;
		case 95: goto st21;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st21;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto st0;
tr31:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
	goto st22;
st22:
	if ( ++p == pe )
		goto _test_eof22;
case 22:
#line 496 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st22;
		case 10: goto tr36;
		case 32: goto st22;
		case 35: goto tr37;
		case 92: goto tr38;
		case 95: goto tr29;
		case 111: goto tr30;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr29;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr29;
	} else
		goto tr29;
	goto st0;
tr32:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st23;
tr36:
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st23;
st23:
	if ( ++p == pe )
		goto _test_eof23;
case 23:
#line 539 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st23;
		case 32: goto st23;
		case 115: goto st24;
	}
	goto st0;
st24:
	if ( ++p == pe )
		goto _test_eof24;
case 24:
	if ( (*p) == 117 )
		goto st25;
	goto st0;
st25:
	if ( ++p == pe )
		goto _test_eof25;
case 25:
	if ( (*p) == 98 )
		goto st26;
	goto st0;
st26:
	if ( ++p == pe )
		goto _test_eof26;
case 26:
	if ( (*p) == 98 )
		goto st27;
	goto st0;
st27:
	if ( ++p == pe )
		goto _test_eof27;
case 27:
	if ( (*p) == 108 )
		goto st28;
	goto st0;
st28:
	if ( ++p == pe )
		goto _test_eof28;
case 28:
	if ( (*p) == 111 )
		goto st29;
	goto st0;
st29:
	if ( ++p == pe )
		goto _test_eof29;
case 29:
	if ( (*p) == 99 )
		goto st30;
	goto st0;
st30:
	if ( ++p == pe )
		goto _test_eof30;
case 30:
	if ( (*p) == 107 )
		goto st31;
	goto st0;
st31:
	if ( ++p == pe )
		goto _test_eof31;
case 31:
	if ( (*p) == 58 )
		goto st32;
	goto st0;
st32:
	if ( ++p == pe )
		goto _test_eof32;
case 32:
	switch( (*p) ) {
		case 9: goto st33;
		case 32: goto st33;
	}
	goto st0;
st33:
	if ( ++p == pe )
		goto _test_eof33;
case 33:
	switch( (*p) ) {
		case 9: goto st33;
		case 32: goto st33;
		case 95: goto tr50;
		case 111: goto tr51;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr50;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr50;
	} else
		goto tr50;
	goto st0;
tr50:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st34;
st34:
	if ( ++p == pe )
		goto _test_eof34;
case 34:
#line 640 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr52;
		case 10: goto tr53;
		case 32: goto tr52;
		case 35: goto tr54;
		case 95: goto st34;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st34;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st34;
	} else
		goto st34;
	goto st0;
tr52:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
	goto st35;
st35:
	if ( ++p == pe )
		goto _test_eof35;
case 35:
#line 671 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st35;
		case 10: goto st132;
		case 32: goto st35;
		case 35: goto st37;
		case 92: goto tr59;
		case 95: goto tr50;
		case 111: goto tr51;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr50;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr50;
	} else
		goto tr50;
	goto st0;
tr53:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
	goto st132;
st132:
	if ( ++p == pe )
		goto _test_eof132;
case 132:
#line 704 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr177;
		case 10: goto tr178;
		case 32: goto tr177;
		case 35: goto tr179;
		case 46: goto tr180;
		case 115: goto st24;
	}
	goto st0;
tr177:
#line 19 "VPRNetParser.rl"
	{
        if(ts != be) {
            ls = buf;
        }
        ts = 0;
        process_clb();
	}
	goto st36;
st36:
	if ( ++p == pe )
		goto _test_eof36;
case 36:
#line 728 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st36;
		case 10: goto st131;
		case 32: goto st36;
		case 35: goto st3;
		case 115: goto st24;
	}
	goto st0;
tr54:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
	goto st37;
st37:
	if ( ++p == pe )
		goto _test_eof37;
case 37:
#line 751 "VPRNetParser.cpp"
	if ( (*p) == 10 )
		goto st132;
	goto st37;
tr59:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st38;
st38:
	if ( ++p == pe )
		goto _test_eof38;
case 38:
#line 765 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr52;
		case 10: goto st35;
		case 32: goto tr52;
		case 35: goto tr54;
		case 95: goto st34;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st34;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st34;
	} else
		goto st34;
	goto st0;
tr51:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st39;
st39:
	if ( ++p == pe )
		goto _test_eof39;
case 39:
#line 792 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr52;
		case 10: goto tr53;
		case 32: goto tr52;
		case 35: goto tr54;
		case 95: goto st34;
		case 112: goto st40;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st34;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st34;
	} else
		goto st34;
	goto st0;
st40:
	if ( ++p == pe )
		goto _test_eof40;
case 40:
	switch( (*p) ) {
		case 9: goto tr52;
		case 10: goto tr53;
		case 32: goto tr52;
		case 35: goto tr54;
		case 95: goto st34;
		case 101: goto st41;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st34;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st34;
	} else
		goto st34;
	goto st0;
st41:
	if ( ++p == pe )
		goto _test_eof41;
case 41:
	switch( (*p) ) {
		case 9: goto tr52;
		case 10: goto tr53;
		case 32: goto tr52;
		case 35: goto tr54;
		case 95: goto st34;
		case 110: goto st42;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st34;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st34;
	} else
		goto st34;
	goto st0;
st42:
	if ( ++p == pe )
		goto _test_eof42;
case 42:
	switch( (*p) ) {
		case 9: goto st35;
		case 10: goto st132;
		case 32: goto st35;
		case 35: goto st37;
		case 92: goto tr59;
		case 95: goto tr50;
		case 111: goto tr64;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr50;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr50;
	} else
		goto tr50;
	goto st0;
tr64:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st43;
st43:
	if ( ++p == pe )
		goto _test_eof43;
case 43:
#line 884 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr52;
		case 10: goto tr53;
		case 32: goto tr52;
		case 35: goto tr54;
		case 95: goto st34;
		case 112: goto st44;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st34;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st34;
	} else
		goto st34;
	goto st0;
st44:
	if ( ++p == pe )
		goto _test_eof44;
case 44:
	switch( (*p) ) {
		case 9: goto tr52;
		case 10: goto tr53;
		case 32: goto tr52;
		case 35: goto tr54;
		case 95: goto st34;
		case 101: goto st45;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st34;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st34;
	} else
		goto st34;
	goto st0;
st45:
	if ( ++p == pe )
		goto _test_eof45;
case 45:
	switch( (*p) ) {
		case 9: goto tr52;
		case 10: goto tr53;
		case 32: goto tr52;
		case 35: goto tr54;
		case 95: goto st34;
		case 110: goto st46;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st34;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st34;
	} else
		goto st34;
	goto st0;
st46:
	if ( ++p == pe )
		goto _test_eof46;
case 46:
	switch( (*p) ) {
		case 9: goto tr52;
		case 10: goto tr53;
		case 32: goto tr52;
		case 35: goto tr54;
		case 92: goto tr59;
		case 95: goto tr50;
		case 111: goto tr64;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr50;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr50;
	} else
		goto tr50;
	goto st0;
tr33:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st47;
tr37:
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st47;
st47:
	if ( ++p == pe )
		goto _test_eof47;
case 47:
#line 990 "VPRNetParser.cpp"
	if ( (*p) == 10 )
		goto st23;
	goto st47;
tr38:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st48;
st48:
	if ( ++p == pe )
		goto _test_eof48;
case 48:
#line 1004 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr31;
		case 10: goto st22;
		case 32: goto tr31;
		case 35: goto tr33;
		case 95: goto st21;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st21;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto st0;
tr30:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st49;
st49:
	if ( ++p == pe )
		goto _test_eof49;
case 49:
#line 1031 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr31;
		case 10: goto tr32;
		case 32: goto tr31;
		case 35: goto tr33;
		case 95: goto st21;
		case 112: goto st50;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st21;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto st0;
st50:
	if ( ++p == pe )
		goto _test_eof50;
case 50:
	switch( (*p) ) {
		case 9: goto tr31;
		case 10: goto tr32;
		case 32: goto tr31;
		case 35: goto tr33;
		case 95: goto st21;
		case 101: goto st51;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st21;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto st0;
st51:
	if ( ++p == pe )
		goto _test_eof51;
case 51:
	switch( (*p) ) {
		case 9: goto tr31;
		case 10: goto tr32;
		case 32: goto tr31;
		case 35: goto tr33;
		case 95: goto st21;
		case 110: goto st52;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st21;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto st0;
st52:
	if ( ++p == pe )
		goto _test_eof52;
case 52:
	switch( (*p) ) {
		case 9: goto st22;
		case 10: goto tr36;
		case 32: goto st22;
		case 35: goto tr37;
		case 92: goto tr38;
		case 95: goto tr29;
		case 111: goto tr72;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr29;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr29;
	} else
		goto tr29;
	goto st0;
tr72:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st53;
st53:
	if ( ++p == pe )
		goto _test_eof53;
case 53:
#line 1123 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr31;
		case 10: goto tr32;
		case 32: goto tr31;
		case 35: goto tr33;
		case 95: goto st21;
		case 112: goto st54;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st21;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto st0;
st54:
	if ( ++p == pe )
		goto _test_eof54;
case 54:
	switch( (*p) ) {
		case 9: goto tr31;
		case 10: goto tr32;
		case 32: goto tr31;
		case 35: goto tr33;
		case 95: goto st21;
		case 101: goto st55;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st21;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto st0;
st55:
	if ( ++p == pe )
		goto _test_eof55;
case 55:
	switch( (*p) ) {
		case 9: goto tr31;
		case 10: goto tr32;
		case 32: goto tr31;
		case 35: goto tr33;
		case 95: goto st21;
		case 110: goto st56;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st21;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st21;
	} else
		goto st21;
	goto st0;
st56:
	if ( ++p == pe )
		goto _test_eof56;
case 56:
	switch( (*p) ) {
		case 9: goto tr31;
		case 10: goto tr32;
		case 32: goto tr31;
		case 35: goto tr33;
		case 92: goto tr38;
		case 95: goto tr29;
		case 111: goto tr72;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr29;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr29;
	} else
		goto tr29;
	goto st0;
tr15:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st57;
st57:
	if ( ++p == pe )
		goto _test_eof57;
case 57:
#line 1219 "VPRNetParser.cpp"
	if ( (*p) == 10 )
		goto st11;
	goto st57;
st58:
	if ( ++p == pe )
		goto _test_eof58;
case 58:
	if ( (*p) == 108 )
		goto st59;
	goto st0;
st59:
	if ( ++p == pe )
		goto _test_eof59;
case 59:
	if ( (*p) == 111 )
		goto st60;
	goto st0;
st60:
	if ( ++p == pe )
		goto _test_eof60;
case 60:
	if ( (*p) == 98 )
		goto st61;
	goto st0;
st61:
	if ( ++p == pe )
		goto _test_eof61;
case 61:
	if ( (*p) == 97 )
		goto st62;
	goto st0;
st62:
	if ( ++p == pe )
		goto _test_eof62;
case 62:
	if ( (*p) == 108 )
		goto st63;
	goto st0;
st63:
	if ( ++p == pe )
		goto _test_eof63;
case 63:
	switch( (*p) ) {
		case 9: goto st64;
		case 32: goto st64;
	}
	goto st0;
st64:
	if ( ++p == pe )
		goto _test_eof64;
case 64:
	switch( (*p) ) {
		case 9: goto st64;
		case 32: goto st64;
		case 95: goto tr82;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr82;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr82;
	} else
		goto tr82;
	goto st0;
tr82:
#line 53 "VPRNetParser.rl"
	{
        label_start = p;
    }
	goto st65;
st65:
	if ( ++p == pe )
		goto _test_eof65;
case 65:
#line 1295 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr83;
		case 10: goto tr84;
		case 32: goto tr83;
		case 35: goto tr85;
		case 95: goto st65;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st65;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st65;
	} else
		goto st65;
	goto st0;
tr83:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st66;
st66:
	if ( ++p == pe )
		goto _test_eof66;
case 66:
#line 1326 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st66;
		case 10: goto st133;
		case 32: goto st66;
		case 35: goto st67;
	}
	goto st0;
tr84:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st133;
st133:
	if ( ++p == pe )
		goto _test_eof133;
case 133:
#line 1348 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st2;
		case 10: goto st131;
		case 32: goto st2;
		case 35: goto st3;
		case 46: goto tr4;
	}
	goto st0;
tr85:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st67;
st67:
	if ( ++p == pe )
		goto _test_eof67;
case 67:
#line 1371 "VPRNetParser.cpp"
	if ( (*p) == 10 )
		goto st133;
	goto st67;
st68:
	if ( ++p == pe )
		goto _test_eof68;
case 68:
	if ( (*p) == 110 )
		goto st69;
	goto st0;
st69:
	if ( ++p == pe )
		goto _test_eof69;
case 69:
	if ( (*p) == 112 )
		goto st70;
	goto st0;
st70:
	if ( ++p == pe )
		goto _test_eof70;
case 70:
	if ( (*p) == 117 )
		goto st71;
	goto st0;
st71:
	if ( ++p == pe )
		goto _test_eof71;
case 71:
	if ( (*p) == 116 )
		goto st72;
	goto st0;
st72:
	if ( ++p == pe )
		goto _test_eof72;
case 72:
	switch( (*p) ) {
		case 9: goto st73;
		case 32: goto st73;
	}
	goto st0;
st73:
	if ( ++p == pe )
		goto _test_eof73;
case 73:
	switch( (*p) ) {
		case 9: goto st73;
		case 32: goto st73;
		case 95: goto tr95;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr95;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr95;
	} else
		goto tr95;
	goto st0;
tr95:
#line 53 "VPRNetParser.rl"
	{
        label_start = p;
    }
	goto st74;
st74:
	if ( ++p == pe )
		goto _test_eof74;
case 74:
#line 1440 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr96;
		case 10: goto tr97;
		case 32: goto tr96;
		case 35: goto tr98;
		case 95: goto st74;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st74;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st74;
	} else
		goto st74;
	goto st0;
tr96:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st75;
st75:
	if ( ++p == pe )
		goto _test_eof75;
case 75:
#line 1471 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st75;
		case 10: goto st76;
		case 32: goto st75;
		case 35: goto st98;
	}
	goto st0;
tr97:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st76;
st76:
	if ( ++p == pe )
		goto _test_eof76;
case 76:
#line 1493 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st76;
		case 32: goto st76;
		case 112: goto tr103;
	}
	goto st0;
tr103:
#line 32 "VPRNetParser.rl"
	{
        pin_list.clear();
        in_pin_list = true;
    }
	goto st77;
st77:
	if ( ++p == pe )
		goto _test_eof77;
case 77:
#line 1511 "VPRNetParser.cpp"
	if ( (*p) == 105 )
		goto st78;
	goto st0;
st78:
	if ( ++p == pe )
		goto _test_eof78;
case 78:
	if ( (*p) == 110 )
		goto st79;
	goto st0;
st79:
	if ( ++p == pe )
		goto _test_eof79;
case 79:
	if ( (*p) == 108 )
		goto st80;
	goto st0;
st80:
	if ( ++p == pe )
		goto _test_eof80;
case 80:
	if ( (*p) == 105 )
		goto st81;
	goto st0;
st81:
	if ( ++p == pe )
		goto _test_eof81;
case 81:
	if ( (*p) == 115 )
		goto st82;
	goto st0;
st82:
	if ( ++p == pe )
		goto _test_eof82;
case 82:
	if ( (*p) == 116 )
		goto st83;
	goto st0;
st83:
	if ( ++p == pe )
		goto _test_eof83;
case 83:
	if ( (*p) == 58 )
		goto st84;
	goto st0;
st84:
	if ( ++p == pe )
		goto _test_eof84;
case 84:
	switch( (*p) ) {
		case 9: goto st85;
		case 32: goto st85;
	}
	goto st0;
st85:
	if ( ++p == pe )
		goto _test_eof85;
case 85:
	switch( (*p) ) {
		case 9: goto st85;
		case 32: goto st85;
		case 95: goto tr112;
		case 111: goto tr113;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr112;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr112;
	} else
		goto tr112;
	goto st0;
tr112:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st86;
st86:
	if ( ++p == pe )
		goto _test_eof86;
case 86:
#line 1595 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr114;
		case 10: goto tr115;
		case 32: goto tr114;
		case 35: goto tr116;
		case 95: goto st86;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st86;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st86;
	} else
		goto st86;
	goto st0;
tr114:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
	goto st87;
st87:
	if ( ++p == pe )
		goto _test_eof87;
case 87:
#line 1626 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st87;
		case 10: goto tr119;
		case 32: goto st87;
		case 35: goto tr120;
		case 92: goto tr121;
		case 95: goto tr112;
		case 111: goto tr113;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr112;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr112;
	} else
		goto tr112;
	goto st0;
tr115:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st134;
tr119:
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st134;
st134:
	if ( ++p == pe )
		goto _test_eof134;
case 134:
#line 1669 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr181;
		case 10: goto tr182;
		case 32: goto tr181;
		case 35: goto tr183;
		case 46: goto tr184;
	}
	goto st0;
tr116:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st88;
tr120:
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st88;
st88:
	if ( ++p == pe )
		goto _test_eof88;
case 88:
#line 1702 "VPRNetParser.cpp"
	if ( (*p) == 10 )
		goto st134;
	goto st88;
tr121:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st89;
st89:
	if ( ++p == pe )
		goto _test_eof89;
case 89:
#line 1716 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr114;
		case 10: goto st87;
		case 32: goto tr114;
		case 35: goto tr116;
		case 95: goto st86;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st86;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st86;
	} else
		goto st86;
	goto st0;
tr113:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st90;
st90:
	if ( ++p == pe )
		goto _test_eof90;
case 90:
#line 1743 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr114;
		case 10: goto tr115;
		case 32: goto tr114;
		case 35: goto tr116;
		case 95: goto st86;
		case 112: goto st91;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st86;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st86;
	} else
		goto st86;
	goto st0;
st91:
	if ( ++p == pe )
		goto _test_eof91;
case 91:
	switch( (*p) ) {
		case 9: goto tr114;
		case 10: goto tr115;
		case 32: goto tr114;
		case 35: goto tr116;
		case 95: goto st86;
		case 101: goto st92;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st86;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st86;
	} else
		goto st86;
	goto st0;
st92:
	if ( ++p == pe )
		goto _test_eof92;
case 92:
	switch( (*p) ) {
		case 9: goto tr114;
		case 10: goto tr115;
		case 32: goto tr114;
		case 35: goto tr116;
		case 95: goto st86;
		case 110: goto st93;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st86;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st86;
	} else
		goto st86;
	goto st0;
st93:
	if ( ++p == pe )
		goto _test_eof93;
case 93:
	switch( (*p) ) {
		case 9: goto st87;
		case 10: goto tr119;
		case 32: goto st87;
		case 35: goto tr120;
		case 92: goto tr121;
		case 95: goto tr112;
		case 111: goto tr127;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr112;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr112;
	} else
		goto tr112;
	goto st0;
tr127:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st94;
st94:
	if ( ++p == pe )
		goto _test_eof94;
case 94:
#line 1835 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr114;
		case 10: goto tr115;
		case 32: goto tr114;
		case 35: goto tr116;
		case 95: goto st86;
		case 112: goto st95;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st86;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st86;
	} else
		goto st86;
	goto st0;
st95:
	if ( ++p == pe )
		goto _test_eof95;
case 95:
	switch( (*p) ) {
		case 9: goto tr114;
		case 10: goto tr115;
		case 32: goto tr114;
		case 35: goto tr116;
		case 95: goto st86;
		case 101: goto st96;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st86;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st86;
	} else
		goto st86;
	goto st0;
st96:
	if ( ++p == pe )
		goto _test_eof96;
case 96:
	switch( (*p) ) {
		case 9: goto tr114;
		case 10: goto tr115;
		case 32: goto tr114;
		case 35: goto tr116;
		case 95: goto st86;
		case 110: goto st97;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st86;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st86;
	} else
		goto st86;
	goto st0;
st97:
	if ( ++p == pe )
		goto _test_eof97;
case 97:
	switch( (*p) ) {
		case 9: goto tr114;
		case 10: goto tr115;
		case 32: goto tr114;
		case 35: goto tr116;
		case 92: goto tr121;
		case 95: goto tr112;
		case 111: goto tr127;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr112;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr112;
	} else
		goto tr112;
	goto st0;
tr98:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st98;
st98:
	if ( ++p == pe )
		goto _test_eof98;
case 98:
#line 1931 "VPRNetParser.cpp"
	if ( (*p) == 10 )
		goto st76;
	goto st98;
st99:
	if ( ++p == pe )
		goto _test_eof99;
case 99:
	if ( (*p) == 117 )
		goto st100;
	goto st0;
st100:
	if ( ++p == pe )
		goto _test_eof100;
case 100:
	if ( (*p) == 116 )
		goto st101;
	goto st0;
st101:
	if ( ++p == pe )
		goto _test_eof101;
case 101:
	if ( (*p) == 112 )
		goto st102;
	goto st0;
st102:
	if ( ++p == pe )
		goto _test_eof102;
case 102:
	if ( (*p) == 117 )
		goto st103;
	goto st0;
st103:
	if ( ++p == pe )
		goto _test_eof103;
case 103:
	if ( (*p) == 116 )
		goto st104;
	goto st0;
st104:
	if ( ++p == pe )
		goto _test_eof104;
case 104:
	switch( (*p) ) {
		case 9: goto st105;
		case 32: goto st105;
	}
	goto st0;
st105:
	if ( ++p == pe )
		goto _test_eof105;
case 105:
	switch( (*p) ) {
		case 9: goto st105;
		case 32: goto st105;
		case 95: goto tr137;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr137;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr137;
	} else
		goto tr137;
	goto st0;
tr137:
#line 53 "VPRNetParser.rl"
	{
        label_start = p;
    }
	goto st106;
st106:
	if ( ++p == pe )
		goto _test_eof106;
case 106:
#line 2007 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr138;
		case 10: goto tr139;
		case 32: goto tr138;
		case 35: goto tr140;
		case 95: goto st106;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st106;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st106;
	} else
		goto st106;
	goto st0;
tr138:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st107;
st107:
	if ( ++p == pe )
		goto _test_eof107;
case 107:
#line 2038 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st107;
		case 10: goto st108;
		case 32: goto st107;
		case 35: goto st130;
	}
	goto st0;
tr139:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st108;
st108:
	if ( ++p == pe )
		goto _test_eof108;
case 108:
#line 2060 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st108;
		case 32: goto st108;
		case 112: goto tr145;
	}
	goto st0;
tr145:
#line 32 "VPRNetParser.rl"
	{
        pin_list.clear();
        in_pin_list = true;
    }
	goto st109;
st109:
	if ( ++p == pe )
		goto _test_eof109;
case 109:
#line 2078 "VPRNetParser.cpp"
	if ( (*p) == 105 )
		goto st110;
	goto st0;
st110:
	if ( ++p == pe )
		goto _test_eof110;
case 110:
	if ( (*p) == 110 )
		goto st111;
	goto st0;
st111:
	if ( ++p == pe )
		goto _test_eof111;
case 111:
	if ( (*p) == 108 )
		goto st112;
	goto st0;
st112:
	if ( ++p == pe )
		goto _test_eof112;
case 112:
	if ( (*p) == 105 )
		goto st113;
	goto st0;
st113:
	if ( ++p == pe )
		goto _test_eof113;
case 113:
	if ( (*p) == 115 )
		goto st114;
	goto st0;
st114:
	if ( ++p == pe )
		goto _test_eof114;
case 114:
	if ( (*p) == 116 )
		goto st115;
	goto st0;
st115:
	if ( ++p == pe )
		goto _test_eof115;
case 115:
	if ( (*p) == 58 )
		goto st116;
	goto st0;
st116:
	if ( ++p == pe )
		goto _test_eof116;
case 116:
	switch( (*p) ) {
		case 9: goto st117;
		case 32: goto st117;
	}
	goto st0;
st117:
	if ( ++p == pe )
		goto _test_eof117;
case 117:
	switch( (*p) ) {
		case 9: goto st117;
		case 32: goto st117;
		case 95: goto tr154;
		case 111: goto tr155;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr154;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr154;
	} else
		goto tr154;
	goto st0;
tr154:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st118;
st118:
	if ( ++p == pe )
		goto _test_eof118;
case 118:
#line 2162 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr156;
		case 10: goto tr157;
		case 32: goto tr156;
		case 35: goto tr158;
		case 95: goto st118;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st118;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st118;
	} else
		goto st118;
	goto st0;
tr156:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
	goto st119;
st119:
	if ( ++p == pe )
		goto _test_eof119;
case 119:
#line 2193 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto st119;
		case 10: goto tr161;
		case 32: goto st119;
		case 35: goto tr162;
		case 92: goto tr163;
		case 95: goto tr154;
		case 111: goto tr155;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr154;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr154;
	} else
		goto tr154;
	goto st0;
tr157:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st135;
tr161:
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st135;
st135:
	if ( ++p == pe )
		goto _test_eof135;
case 135:
#line 2236 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr185;
		case 10: goto tr186;
		case 32: goto tr185;
		case 35: goto tr187;
		case 46: goto tr188;
	}
	goto st0;
tr158:
#line 45 "VPRNetParser.rl"
	{
        length = p - pin_start;
        if(length < 0) {
            pin_start = buf + (pin_start - be);
        }
        if(in_pin_list) pin_list.push_back(string(pin_start, p - pin_start));
    }
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st120;
tr162:
#line 37 "VPRNetParser.rl"
	{
        in_pin_list = false;
    }
	goto st120;
st120:
	if ( ++p == pe )
		goto _test_eof120;
case 120:
#line 2269 "VPRNetParser.cpp"
	if ( (*p) == 10 )
		goto st135;
	goto st120;
tr163:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st121;
st121:
	if ( ++p == pe )
		goto _test_eof121;
case 121:
#line 2283 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr156;
		case 10: goto st119;
		case 32: goto tr156;
		case 35: goto tr158;
		case 95: goto st118;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st118;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st118;
	} else
		goto st118;
	goto st0;
tr155:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st122;
st122:
	if ( ++p == pe )
		goto _test_eof122;
case 122:
#line 2310 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr156;
		case 10: goto tr157;
		case 32: goto tr156;
		case 35: goto tr158;
		case 95: goto st118;
		case 112: goto st123;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st118;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st118;
	} else
		goto st118;
	goto st0;
st123:
	if ( ++p == pe )
		goto _test_eof123;
case 123:
	switch( (*p) ) {
		case 9: goto tr156;
		case 10: goto tr157;
		case 32: goto tr156;
		case 35: goto tr158;
		case 95: goto st118;
		case 101: goto st124;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st118;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st118;
	} else
		goto st118;
	goto st0;
st124:
	if ( ++p == pe )
		goto _test_eof124;
case 124:
	switch( (*p) ) {
		case 9: goto tr156;
		case 10: goto tr157;
		case 32: goto tr156;
		case 35: goto tr158;
		case 95: goto st118;
		case 110: goto st125;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st118;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st118;
	} else
		goto st118;
	goto st0;
st125:
	if ( ++p == pe )
		goto _test_eof125;
case 125:
	switch( (*p) ) {
		case 9: goto st119;
		case 10: goto tr161;
		case 32: goto st119;
		case 35: goto tr162;
		case 92: goto tr163;
		case 95: goto tr154;
		case 111: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr154;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr154;
	} else
		goto tr154;
	goto st0;
tr169:
#line 41 "VPRNetParser.rl"
	{
        pin_start = p;
    }
	goto st126;
st126:
	if ( ++p == pe )
		goto _test_eof126;
case 126:
#line 2402 "VPRNetParser.cpp"
	switch( (*p) ) {
		case 9: goto tr156;
		case 10: goto tr157;
		case 32: goto tr156;
		case 35: goto tr158;
		case 95: goto st118;
		case 112: goto st127;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st118;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st118;
	} else
		goto st118;
	goto st0;
st127:
	if ( ++p == pe )
		goto _test_eof127;
case 127:
	switch( (*p) ) {
		case 9: goto tr156;
		case 10: goto tr157;
		case 32: goto tr156;
		case 35: goto tr158;
		case 95: goto st118;
		case 101: goto st128;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st118;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st118;
	} else
		goto st118;
	goto st0;
st128:
	if ( ++p == pe )
		goto _test_eof128;
case 128:
	switch( (*p) ) {
		case 9: goto tr156;
		case 10: goto tr157;
		case 32: goto tr156;
		case 35: goto tr158;
		case 95: goto st118;
		case 110: goto st129;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto st118;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto st118;
	} else
		goto st118;
	goto st0;
st129:
	if ( ++p == pe )
		goto _test_eof129;
case 129:
	switch( (*p) ) {
		case 9: goto tr156;
		case 10: goto tr157;
		case 32: goto tr156;
		case 35: goto tr158;
		case 92: goto tr163;
		case 95: goto tr154;
		case 111: goto tr169;
	}
	if ( (*p) < 65 ) {
		if ( 48 <= (*p) && (*p) <= 58 )
			goto tr154;
	} else if ( (*p) > 93 ) {
		if ( 97 <= (*p) && (*p) <= 122 )
			goto tr154;
	} else
		goto tr154;
	goto st0;
tr140:
#line 57 "VPRNetParser.rl"
	{
        length = p - label_start;
        if(length < 0) {
            label_start = buf + (label_start - be);
        }
        label = string(label_start, p - label_start);
    }
	goto st130;
st130:
	if ( ++p == pe )
		goto _test_eof130;
case 130:
#line 2498 "VPRNetParser.cpp"
	if ( (*p) == 10 )
		goto st108;
	goto st130;
	}
	_test_eof2: cs = 2; goto _test_eof; 
	_test_eof131: cs = 131; goto _test_eof; 
	_test_eof3: cs = 3; goto _test_eof; 
	_test_eof4: cs = 4; goto _test_eof; 
	_test_eof5: cs = 5; goto _test_eof; 
	_test_eof6: cs = 6; goto _test_eof; 
	_test_eof7: cs = 7; goto _test_eof; 
	_test_eof8: cs = 8; goto _test_eof; 
	_test_eof9: cs = 9; goto _test_eof; 
	_test_eof10: cs = 10; goto _test_eof; 
	_test_eof11: cs = 11; goto _test_eof; 
	_test_eof12: cs = 12; goto _test_eof; 
	_test_eof13: cs = 13; goto _test_eof; 
	_test_eof14: cs = 14; goto _test_eof; 
	_test_eof15: cs = 15; goto _test_eof; 
	_test_eof16: cs = 16; goto _test_eof; 
	_test_eof17: cs = 17; goto _test_eof; 
	_test_eof18: cs = 18; goto _test_eof; 
	_test_eof19: cs = 19; goto _test_eof; 
	_test_eof20: cs = 20; goto _test_eof; 
	_test_eof21: cs = 21; goto _test_eof; 
	_test_eof22: cs = 22; goto _test_eof; 
	_test_eof23: cs = 23; goto _test_eof; 
	_test_eof24: cs = 24; goto _test_eof; 
	_test_eof25: cs = 25; goto _test_eof; 
	_test_eof26: cs = 26; goto _test_eof; 
	_test_eof27: cs = 27; goto _test_eof; 
	_test_eof28: cs = 28; goto _test_eof; 
	_test_eof29: cs = 29; goto _test_eof; 
	_test_eof30: cs = 30; goto _test_eof; 
	_test_eof31: cs = 31; goto _test_eof; 
	_test_eof32: cs = 32; goto _test_eof; 
	_test_eof33: cs = 33; goto _test_eof; 
	_test_eof34: cs = 34; goto _test_eof; 
	_test_eof35: cs = 35; goto _test_eof; 
	_test_eof132: cs = 132; goto _test_eof; 
	_test_eof36: cs = 36; goto _test_eof; 
	_test_eof37: cs = 37; goto _test_eof; 
	_test_eof38: cs = 38; goto _test_eof; 
	_test_eof39: cs = 39; goto _test_eof; 
	_test_eof40: cs = 40; goto _test_eof; 
	_test_eof41: cs = 41; goto _test_eof; 
	_test_eof42: cs = 42; goto _test_eof; 
	_test_eof43: cs = 43; goto _test_eof; 
	_test_eof44: cs = 44; goto _test_eof; 
	_test_eof45: cs = 45; goto _test_eof; 
	_test_eof46: cs = 46; goto _test_eof; 
	_test_eof47: cs = 47; goto _test_eof; 
	_test_eof48: cs = 48; goto _test_eof; 
	_test_eof49: cs = 49; goto _test_eof; 
	_test_eof50: cs = 50; goto _test_eof; 
	_test_eof51: cs = 51; goto _test_eof; 
	_test_eof52: cs = 52; goto _test_eof; 
	_test_eof53: cs = 53; goto _test_eof; 
	_test_eof54: cs = 54; goto _test_eof; 
	_test_eof55: cs = 55; goto _test_eof; 
	_test_eof56: cs = 56; goto _test_eof; 
	_test_eof57: cs = 57; goto _test_eof; 
	_test_eof58: cs = 58; goto _test_eof; 
	_test_eof59: cs = 59; goto _test_eof; 
	_test_eof60: cs = 60; goto _test_eof; 
	_test_eof61: cs = 61; goto _test_eof; 
	_test_eof62: cs = 62; goto _test_eof; 
	_test_eof63: cs = 63; goto _test_eof; 
	_test_eof64: cs = 64; goto _test_eof; 
	_test_eof65: cs = 65; goto _test_eof; 
	_test_eof66: cs = 66; goto _test_eof; 
	_test_eof133: cs = 133; goto _test_eof; 
	_test_eof67: cs = 67; goto _test_eof; 
	_test_eof68: cs = 68; goto _test_eof; 
	_test_eof69: cs = 69; goto _test_eof; 
	_test_eof70: cs = 70; goto _test_eof; 
	_test_eof71: cs = 71; goto _test_eof; 
	_test_eof72: cs = 72; goto _test_eof; 
	_test_eof73: cs = 73; goto _test_eof; 
	_test_eof74: cs = 74; goto _test_eof; 
	_test_eof75: cs = 75; goto _test_eof; 
	_test_eof76: cs = 76; goto _test_eof; 
	_test_eof77: cs = 77; goto _test_eof; 
	_test_eof78: cs = 78; goto _test_eof; 
	_test_eof79: cs = 79; goto _test_eof; 
	_test_eof80: cs = 80; goto _test_eof; 
	_test_eof81: cs = 81; goto _test_eof; 
	_test_eof82: cs = 82; goto _test_eof; 
	_test_eof83: cs = 83; goto _test_eof; 
	_test_eof84: cs = 84; goto _test_eof; 
	_test_eof85: cs = 85; goto _test_eof; 
	_test_eof86: cs = 86; goto _test_eof; 
	_test_eof87: cs = 87; goto _test_eof; 
	_test_eof134: cs = 134; goto _test_eof; 
	_test_eof88: cs = 88; goto _test_eof; 
	_test_eof89: cs = 89; goto _test_eof; 
	_test_eof90: cs = 90; goto _test_eof; 
	_test_eof91: cs = 91; goto _test_eof; 
	_test_eof92: cs = 92; goto _test_eof; 
	_test_eof93: cs = 93; goto _test_eof; 
	_test_eof94: cs = 94; goto _test_eof; 
	_test_eof95: cs = 95; goto _test_eof; 
	_test_eof96: cs = 96; goto _test_eof; 
	_test_eof97: cs = 97; goto _test_eof; 
	_test_eof98: cs = 98; goto _test_eof; 
	_test_eof99: cs = 99; goto _test_eof; 
	_test_eof100: cs = 100; goto _test_eof; 
	_test_eof101: cs = 101; goto _test_eof; 
	_test_eof102: cs = 102; goto _test_eof; 
	_test_eof103: cs = 103; goto _test_eof; 
	_test_eof104: cs = 104; goto _test_eof; 
	_test_eof105: cs = 105; goto _test_eof; 
	_test_eof106: cs = 106; goto _test_eof; 
	_test_eof107: cs = 107; goto _test_eof; 
	_test_eof108: cs = 108; goto _test_eof; 
	_test_eof109: cs = 109; goto _test_eof; 
	_test_eof110: cs = 110; goto _test_eof; 
	_test_eof111: cs = 111; goto _test_eof; 
	_test_eof112: cs = 112; goto _test_eof; 
	_test_eof113: cs = 113; goto _test_eof; 
	_test_eof114: cs = 114; goto _test_eof; 
	_test_eof115: cs = 115; goto _test_eof; 
	_test_eof116: cs = 116; goto _test_eof; 
	_test_eof117: cs = 117; goto _test_eof; 
	_test_eof118: cs = 118; goto _test_eof; 
	_test_eof119: cs = 119; goto _test_eof; 
	_test_eof135: cs = 135; goto _test_eof; 
	_test_eof120: cs = 120; goto _test_eof; 
	_test_eof121: cs = 121; goto _test_eof; 
	_test_eof122: cs = 122; goto _test_eof; 
	_test_eof123: cs = 123; goto _test_eof; 
	_test_eof124: cs = 124; goto _test_eof; 
	_test_eof125: cs = 125; goto _test_eof; 
	_test_eof126: cs = 126; goto _test_eof; 
	_test_eof127: cs = 127; goto _test_eof; 
	_test_eof128: cs = 128; goto _test_eof; 
	_test_eof129: cs = 129; goto _test_eof; 
	_test_eof130: cs = 130; goto _test_eof; 

	_test_eof: {}
	if ( p == eof )
	{
	switch ( cs ) {
	case 134: 
#line 11 "VPRNetParser.rl"
	{
        process_input();
	}
	break;
	case 135: 
#line 15 "VPRNetParser.rl"
	{
        process_output();
	}
	break;
	case 132: 
#line 19 "VPRNetParser.rl"
	{
        if(ts != be) {
            ls = buf;
        }
        ts = 0;
        process_clb();
	}
	break;
	case 131: 
#line 27 "VPRNetParser.rl"
	{
        be = p;
        te = p;
	}
	break;
#line 2671 "VPRNetParser.cpp"
	}
	}

	_out: {}
	}

#line 140 "VPRNetParser.rl"

            if ( cs == VPRNetParser_error ) {
                /* Machine failed before finding a token. */
                cerr << "PARSE ERROR" << endl;
                exit(1);
            }
            if ( ts == 0 ) {
                have = 0;
            } else {
                /* There is a prefix to preserve, shift it over. */
                have = pe - ts;
                memmove( buf, ts, have );
                te = buf + (te-ts);
                ts = buf;
            }
        }
    }
}
