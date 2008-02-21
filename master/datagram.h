/******************************************************************************
 *
 *  $Id$
 *
 *  Copyright (C) 2006  Florian Pose, Ingenieurgemeinschaft IgH
 *
 *  This file is part of the IgH EtherCAT Master.
 *
 *  The IgH EtherCAT Master is free software; you can redistribute it
 *  and/or modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  The IgH EtherCAT Master is distributed in the hope that it will be
 *  useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the IgH EtherCAT Master; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  The right to use EtherCAT Technology is granted and comes free of
 *  charge under condition of compatibility of product made by
 *  Licensee. People intending to distribute/sell products based on the
 *  code, have to sign an agreement to guarantee that products using
 *  software based on IgH EtherCAT master stay compatible with the actual
 *  EtherCAT specification (which are released themselves as an open
 *  standard) as the (only) precondition to have the right to use EtherCAT
 *  Technology, IP and trade marks.
 *
 *****************************************************************************/

/**
   \file
   EtherCAT datagram structure.
*/

/*****************************************************************************/

#ifndef _EC_DATAGRAM_H_
#define _EC_DATAGRAM_H_

#include <linux/list.h>
#include <linux/time.h>
#include <linux/timex.h>

#include "globals.h"

/*****************************************************************************/

/** size of the datagram description string */
#define EC_DATAGRAM_NAME_SIZE 20

/*****************************************************************************/

/** EtherCAT datagram type.
 */
typedef enum {
    EC_DATAGRAM_NONE = 0x00, /**< Dummy. */
    EC_DATAGRAM_APRD = 0x01, /**< Auto-increment physical read. */
    EC_DATAGRAM_APWR = 0x02, /**< Auto-increment physical write. */
    EC_DATAGRAM_NPRD = 0x04, /**< Node-addressed physical read. */
    EC_DATAGRAM_NPWR = 0x05, /**< Node-addressed physical write. */
    EC_DATAGRAM_BRD  = 0x07, /**< Broadcast read. */
    EC_DATAGRAM_BWR  = 0x08, /**< Broadcast write. */
    EC_DATAGRAM_LRW  = 0x0C  /**< Logical read/write. */
} ec_datagram_type_t;

/*****************************************************************************/

/** EtherCAT datagram state.
 */
typedef enum {
    EC_DATAGRAM_INIT,      /**< Initial state of a new datagram. */
    EC_DATAGRAM_QUEUED,    /**< Queued for sending. */
    EC_DATAGRAM_SENT,      /**< Sent (still in the queue). */
    EC_DATAGRAM_RECEIVED,  /**< Received (dequeued). */
    EC_DATAGRAM_TIMED_OUT, /**< Timed out (dequeued). */
    EC_DATAGRAM_ERROR      /**< Error while sending/receiving (dequeued). */
} ec_datagram_state_t;

/*****************************************************************************/

/** EtherCAT datagram.
 */
typedef struct {
    struct list_head list; /**< Needed by domain datagram lists. */
    struct list_head queue; /**< Master datagram queue item. */
    struct list_head sent; /**< Master list item for sent datagrams. */
    ec_datagram_type_t type; /**< Datagram type (APRD, BWR, etc.). */
    uint8_t address[EC_ADDR_LEN]; /**< Recipient address. */
    uint8_t *data; /**< Datagram payload. */
    ec_origin_t data_origin; /**< Origin of the \a data memory. */
    size_t mem_size; /**< Datagram \a data memory size. */
    size_t data_size; /**< Size of the data in \a data. */
    uint8_t index; /**< Index (set by master). */
    uint16_t working_counter; /**< Working counter. */
    ec_datagram_state_t state; /**< State. */
    cycles_t cycles_sent; /**< Time, when the datagram was sent. */
    unsigned long jiffies_sent; /**< Jiffies, when the datagram was sent. */
    cycles_t cycles_received; /**< Time, when the datagram was received. */
    unsigned long jiffies_received; /**< Jiffies, when the datagram was
                                      received. */
    unsigned int skip_count; /**< Number of requeues when not yet received. */
    unsigned long stats_output_jiffies; /**< Last statistics output. */
    char name[EC_DATAGRAM_NAME_SIZE]; /**< Description of the datagram. */
} ec_datagram_t;

/*****************************************************************************/

void ec_datagram_init(ec_datagram_t *);
void ec_datagram_clear(ec_datagram_t *);
int ec_datagram_prealloc(ec_datagram_t *, size_t);

int ec_datagram_nprd(ec_datagram_t *, uint16_t, uint16_t, size_t);
int ec_datagram_npwr(ec_datagram_t *, uint16_t, uint16_t, size_t);
int ec_datagram_aprd(ec_datagram_t *, uint16_t, uint16_t, size_t);
int ec_datagram_apwr(ec_datagram_t *, uint16_t, uint16_t, size_t);
int ec_datagram_brd(ec_datagram_t *, uint16_t, size_t);
int ec_datagram_bwr(ec_datagram_t *, uint16_t, size_t);
int ec_datagram_lrw(ec_datagram_t *, uint32_t, size_t, uint8_t *);

void ec_datagram_print_wc_error(const ec_datagram_t *);
void ec_datagram_output_stats(ec_datagram_t *datagram);

/*****************************************************************************/

#endif
