
#include <string.h>

#include <arpa/inet.h>

#include <glib.h>

#include <udp.h>
#include <udp-fake.h>

int
main (void)
{
  UDPSocketManager man;
  UDPSocket sock;
  struct sockaddr_in sin;
  guint len;
  gchar buf[1024];

  udp_fake_socket_manager_init (&man);

  memset (buf, '\0', 1024);

  /* create fake socket */

  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = 0;
  udp_socket_manager_alloc_socket (&man, &sock, &sin);

  /* test recv */

  memcpy (buf, "he\0lo", 5);
  len = 5;
  sin.sin_addr.s_addr = htonl (0x01020304);
  sin.sin_port = htons (2345);
  udp_fake_socket_push_recv (&sock, &sin, len, buf);

  memset (buf, '\0', 5);
  memset (&sin, '\0', sizeof (sin));

  len = udp_socket_recv (&sock, &sin, sizeof (buf), buf);
  g_assert (len == 5);
  g_assert (memcmp (buf, "he\0lo", 5) == 0);
  g_assert (ntohl (sin.sin_addr.s_addr) == 0x01020304);
  g_assert (ntohs (sin.sin_port) == 2345);

  /* test send */

  memcpy (buf, "la\0la", 5);
  len = 5;
  udp_socket_send (&sock, &sin, len, buf);

  memset (buf, '\0', len);
  memset (&sin, '\0', sizeof (sin));

  len = udp_fake_socket_pop_send (&sock, &sin, sizeof (buf), buf);
  g_assert (len == 5);
  g_assert (0 == memcmp (buf, "la\0la", 5));
  g_assert (ntohl (sin.sin_addr.s_addr) == 0x01020304);
  g_assert (ntohs (sin.sin_port) == 2345);

  udp_socket_close (&sock);
  udp_socket_manager_close (&man);
  return 0;
}

