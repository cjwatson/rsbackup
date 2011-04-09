#ifndef CONF_H
#define CONF_H

#include <map>
#include <string>
#include <vector>
#include <climits>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "Defaults.h"

class Store;
class Device;
class Host;
class Volume;

// Volume, Host and Conf share certain parameters, which are inherited from
// container to contained.  They are implemented in a ConfBase class.
class ConfBase {
public:
  ConfBase(): maxAge(DEFAULT_MAX_AGE),
              minBackups(DEFAULT_MIN_BACKUPS),
              pruneAge(DEFAULT_PRUNE_AGE) {}
  ConfBase(ConfBase *parent): maxAge(parent->maxAge),
                              minBackups(parent->minBackups),
                              pruneAge(parent->pruneAge) {}
  int maxAge;
  int minBackups;
  int pruneAge;
};

typedef std::map<std::string,Host *> hosts_type;
typedef std::map<std::string,Store *> stores_type;
typedef std::map<std::string,Device *> devices_type;

// Represents the entire configuration of rsbackup.
class Conf: public ConfBase {
public:
  Conf(): maxUsage(DEFAULT_MAX_USAGE),
          maxFileUsage(DEFAULT_MAX_FILE_USAGE),
          publicStores(false),
          logs(DEFAULT_LOGS),
          sshTimeout(DEFAULT_SSH_TIMEOUT),
          keepPruneLogs(DEFAULT_KEEP_PRUNE_LOGS),
          sendmail(DEFAULT_SENDMAIL) { }
  hosts_type hosts;
  stores_type stores;
  devices_type devices;
  int maxUsage;
  int maxFileUsage;
  bool publicStores;
  std::string logs;
  std::string lock;
  int sshTimeout;
  int keepPruneLogs;
  std::string sendmail;

  void read();

  void selectVolume(const std::string &hostName,
                    const std::string &volumeName,
                    bool sense = true);

private:
  void readOneFile(const std::string &path);
  void includeFile(const std::string &path);
  static void split(std::vector<std::string> &bits, const std::string &line);
  static int parseInteger(const std::string &s,
                          int min = INT_MIN, int max = INT_MAX);
  void selectAll(bool sense = true);
  void selectHost(const std::string &hostName,
                  bool sense = true);
};

// Represents a backup device.
class Device {
public:
  Device(const std::string &name_): name(name_), store(NULL) {}
  std::string name;
  Store *store;                         // store for this device, or NULL
};

typedef std::map<std::string,Volume *> volumes_type;

// Represents a host to back up; a container for volumes.
class Host: public ConfBase {
public:
  Host(Conf *parent, const std::string &name_):
    ConfBase(static_cast<ConfBase *>(parent)),
    name(name_),
    hostname(name_) {}
  std::string name;
  volumes_type volumes;
  std::string user;
  std::string hostname;

  bool selected() const;                // true if any volume selected
  void select(bool sense);              // (de-)select all volumes
};

// Represents a single volume (usually, filesystem) to back up.
class Volume: public ConfBase {
public:
  Volume(Host *parent,
         const std::string &name_,
         const std::string &path_): ConfBase(static_cast<ConfBase *>(parent)),
                                    name(name_),
                                    path(path_),
                                    traverse(false),
                                    isSelected(false) {}
  std::string name;
  std::string path;
  std::vector<std::string> exclude;
  bool traverse;

  bool selected() const { return isSelected; }
  void select(bool sense);

private:
  bool isSelected;
};

extern Conf config;

#endif /* CONF_H */

/*
Local Variables:
mode:c++
c-basic-offset:2
comment-column:40
fill-column:79
indent-tabs-mode:nil
End:
*/
