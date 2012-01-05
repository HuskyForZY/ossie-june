/****************************************************************************

Copyright 2007 Virginia Polytechnic Institute and State University

This file is part of the OSSIE FrameSynchronizer.

OSSIE FrameSynchronizer is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

OSSIE FrameSynchronizer is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with OSSIE FrameSynchronizer; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

****************************************************************************/

/// \brief P/N frame synchronization code
///
/// Maximal length bit sequence for synchronizing the receiver with the
/// beginning of the frame.
///
/// generator polynomial : g = 435 (octal)
extern char pnFrameSyncCode[255];

/// \brief P/N frame end-of-message (EOM) code
///
/// Maximal length bit sequence for synchronizing the receiver with the
/// end of the frame.
///
/// generator polynomial : g = 453 (octal)
extern char pnFrameEOMCode[255];

/// \brief P/N control code
///
/// Maximal length bit sequence for relaying basic control information.
///
/// generator polynomial : g = 45 (octal)
extern char pnControlCode[31];

/// \brief Inverse P/N control code
///
/// Inverse of pnControlCode
extern char pnControlCodeInv[31];

