/* Virtual File System: Samba file system.
   The internal functions

   Copyright (C) 2013
   The Free Software Foundation, Inc.

   Written by:
   Slava Zanko <slavazanko@gmail.com>, 2013

   This file is part of the Midnight Commander.

   The Midnight Commander is free software: you can redistribute it
   and/or modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation, either version 3 of the License,
   or (at your option) any later version.

   The Midnight Commander is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include <errno.h>

#include "lib/global.h"

#include "internal.h"

/*** global variables ****************************************************************************/

/*** file scope macro definitions ****************************************************************/

#define SMBFS_ERRORS_PREFIX "smbfs: "
#define SMBFS_ERRORS_PREFIX_LEN sizeof(SMBFS_ERRORS_PREFIX)

/*** file scope type declarations ****************************************************************/

/*** file scope variables ************************************************************************/

/*** file scope functions ************************************************************************/
/* --------------------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------------------- */
/*** public functions ****************************************************************************/
/* --------------------------------------------------------------------------------------------- */

void
smbfs_cb_authdata_provider (const char *server, const char *share,
                            char *workgroup, int wgmaxlen, char *username, int unmaxlen,
                            char *password, int pwmaxlen)
{
    smbfs_auth_dialog (server, share, workgroup, wgmaxlen, username, unmaxlen, password, pwmaxlen);
}

/* --------------------------------------------------------------------------------------------- */
/**
 * Get error string for SAMBA connections by error number.
 *
 * @param err_no error number
 * @return the string representation of the error
 */
const char *
smbfs_strerror (int err_no)
{
    const char *error_str;

    switch (err_no)
    {
    case EINVAL:
        error_str = _("smbfs: an incorrect form of file/URL was passed");
        break;
    case EPERM:
    case ENODEV:
        error_str = _("smbfs: the workgroup or server could not be found");
        break;
    default:
        {
            static char sys_error_str[BUF_LARGE];
            strcpy (sys_error_str, SMBFS_ERRORS_PREFIX);
            strncpy (sys_error_str + SMBFS_ERRORS_PREFIX_LEN - 1, g_strerror (err_no),
                     BUF_LARGE - SMBFS_ERRORS_PREFIX_LEN);
            sys_error_str[BUF_LARGE - 1] = '\0';
            error_str = sys_error_str;
        }
    }
    return error_str;
}

/* --------------------------------------------------------------------------------------------- */

char *
smbfs_make_url (const vfs_path_element_t * element, gboolean with_path)
{
    GString *buffer;

    if (element == NULL)
        return NULL;

    buffer = g_string_new ("smb://");

    if (element->user != NULL)
    {
        g_string_append (buffer, element->user);

        if (element->password != NULL)
        {
            g_string_append_c (buffer, ':');
            g_string_append (buffer, element->password);
        }
    }

    if (element->host != NULL)
    {
        if ((element->user != NULL))
            g_string_append_c (buffer, '@');
        if (element->ipv6)
            g_string_append_c (buffer, '[');
        g_string_append (buffer, element->host);
        if (element->ipv6)
            g_string_append_c (buffer, ']');
    }

    if (*element->path != '\0')
    {
        if (*(buffer->str + buffer->len) != PATH_SEP)
            g_string_append_c (buffer, PATH_SEP);

        if (with_path)
            g_string_append (buffer, element->path);
        else
        {
            const char *slash_pointer;

            slash_pointer = strchr (element->path, PATH_SEP);
            if (slash_pointer == NULL)
                g_string_append (buffer, element->path);
            else
                g_string_append_len (buffer, element->path, slash_pointer - element->path);
        }
    }

    return g_string_free (buffer, FALSE);
}

/* --------------------------------------------------------------------------------------------- */