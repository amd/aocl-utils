/*
 * Copyright (C) 2023, Advanced Micro Devices. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 * without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "Au/Au.hh"
#include "Au/Base64.hh"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace Au;
using namespace std;

void
demo_string()
{
}

void
demo_stream(ifstream const& fin)
{
    /* Read file contents into a buffer */
    std::string strng{};
    strng.reserve(fin.tellg());
    fin.read(strng.data(), strng.size());

    /* Initialize stream with string read */
    istringstream sin{ strng };

    /* Setup an output stream */
    ostringstream sout;
    sout.clear(); /* First clear any default data */
    sout.str(""); /* Initialize with empty string */

    Base64Encoder encoder{ sout };

    encoder.encode(sin);
}

void
demo_reader_writer(ifstream const& fin)
{
}

int
main(int argc, char* argv[])
{
    if (argc != 2) {
        cout << "Filename should be an argument to this program"
             << "\n";
        cout << "This program reads file from argv[1] and encodes in\n"
             << "Base64 format"
             << "\n";
        goto fail_out;
    }

    /* Open the file in binary */
    ifstream fin{ argv[1], ios::binary };
    if (!fin) {
        cout << "can't open file " << argv[1] << "\n";
        return EXIT_FAILURE;
    }

    demo_string();

    demo_stream(fin);

    demo_reader_writer(fin);

    /* Close the file */
    fin.close();

    return EXIT_SUCCESS;

fail_out:
    return EXIT_FAILURE;
}
