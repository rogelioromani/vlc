/*****************************************************************************
 * id3.c: simple id3 tag skipper
 *****************************************************************************
 * Copyright (C) 2001 VideoLAN
 * $Id: id3.c,v 1.5 2003/10/25 00:49:14 sam Exp $
 *
 * Authors: Sigmund Augdal <sigmunau@idi.ntnu.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111, USA.
 *****************************************************************************/

/*****************************************************************************
 * Preamble
 *****************************************************************************/
#include <stdlib.h>                                      /* malloc(), free() */
#include <string.h>

#include <vlc/vlc.h>
#include <vlc/input.h>

#include <sys/types.h>

/*****************************************************************************
 * Local prototypes
 *****************************************************************************/
static int  SkipID3Tag ( vlc_object_t * );

/*****************************************************************************
 * Module descriptor
 *****************************************************************************/
vlc_module_begin();
    set_description( _("Simple id3 tag skipper" ) );
    set_capability( "id3", 50 );
    set_callbacks( SkipID3Tag, NULL );
vlc_module_end();

/****************************************************************************
 * SkipID3Tag : check if an ID3 tag is present, and skip it if it is
 ****************************************************************************/
static int SkipID3Tag( vlc_object_t *p_this )
{
    input_thread_t *p_input;
    uint8_t *p_peek;
    int i_size;
    uint8_t version, revision;
    int b_footer;

    if ( p_this->i_object_type != VLC_OBJECT_INPUT )
    {
        return( VLC_EGENERIC );
    }
    p_input = (input_thread_t *)p_this;

    msg_Dbg( p_input, "Checking for ID3 tag" );
    /* get 10 byte id3 header */
    if( input_Peek( p_input, &p_peek, 10 ) < 10 )
    {
        msg_Err( p_input, "cannot peek()" );
        return( VLC_EGENERIC );
    }

    if ( !( (p_peek[0] == 0x49) && (p_peek[1] == 0x44) && (p_peek[2] == 0x33)))
    {
        return( VLC_SUCCESS );
    }

    version = p_peek[3];  /* These may become usfull later, */
    revision = p_peek[4]; /* but we ignore them for now */
    b_footer = p_peek[5] & 0x10;
    i_size = (p_peek[6] << 21) +
             (p_peek[7] << 14) +
             (p_peek[8] << 7) +
             p_peek[9];
    if ( b_footer )
    {
        i_size += 10;
    }
    i_size += 10;

    /* peek the entire tag */
    if ( input_Peek( p_input, &p_peek, i_size ) < i_size )
    {
        msg_Err( p_input, "cannot peek()" );
        return( VLC_EGENERIC );
    }

    msg_Dbg( p_input, "ID3v2.%d revision %d tag found, skiping %d bytes",
             version, revision, i_size );
    p_input->p_current_data += i_size; /* seek passed end of ID3 tag */
    return ( VLC_SUCCESS );
}
