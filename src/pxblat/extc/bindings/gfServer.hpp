#ifndef GF_SERVER_HPP
#define GF_SERVER_HPP

#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/socket.h>

#include <cstring>
#include <ctime>
#include <ios>
#include <optional>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "common.h"
#include "dnaseq.h"
#include "dnautil.h"
#include "dystring.h"
#include "errAbort.h"
#include "fa.h"
#include "filePath.h"
#include "genoFind.h"
#include "hash.h"
#include "internet.h"
#include "log.h"
#include "memalloc.h"
#include "netlib.h"
#include "nib.h"
#include "portable.h"
#include "trans3.h"
#include "twoBit.h"

namespace cppbinding {

struct Signal {
  bool isReady{};
  Signal() = default;
  friend std::ostream &operator<<(std::ostream &os, const Signal &signal);
};

struct UsageStats {
  long baseCount{0}, blatCount{0}, aaCount{0}, pcrCount{0};
  int warnCount{0};
  int noSigCount{0};
  int missCount{0};
  int trimCount{0};
  UsageStats() = default;
  friend std::ostream &operator<<(std::ostream &os, const UsageStats &stats);
};

// constexpr int stepSize = 0; /* Can be overridden from command line. */
// boolean doTrans = FALSE; /* Do translation? */
// boolean allowOneMismatch = FALSE;
// boolean noSimpRepMask = FALSE;
// constexpr int repMatch = 1024; /* Can be overridden from command line. */
// constexpr int maxGap = gfMaxGap;
// boolean seqLog = FALSE;
// boolean ipLog = FALSE;
// boolean doMask = FALSE;
// boolean canStop = FALSE;
// char *indexFile = NULL;
// char *genome = NULL;
// char *genomeDataDir = NULL;

// constexpr int timeout = 90;  // default timeout in seconds

// {self.gfserver} -canStop -log={self.log_file_path} -stepSize=5 start "
//             f"localhost {self.port} {os.path.basename(self.ref_2bit)}"

struct gfServerOption {
  using self = gfServerOption;
  bool canStop{false};        //-canStop      If set, a quit message will actually take down the server.
  std::string log{};          //-log=logFile  Keep a log file that records server requests.
  std::string logFacility{};  //-logFacility=facility  Log to the specified syslog facility default local0
  bool mask{};                //-mask   Use masking from .2bit file.
  int maxAaSize{8000};        //-maxAaSize=N  Maximum size of protein or translated DNA queries."
  int maxDnaHits{100};        //-maxDnaHits=N   Maximum number of hits for a DNA query that are sent from the server"
  int maxGap{gfMaxGap};       //-maxGap=N    Number of insertions or deletions allowed between n-mers. Default is 2 for
                              // nucleotides, 0 for proteins"
  int maxNtSize{40000};       //-maxNtSize=N    Maximum size of untranslated DNA query sequence
  int maxTransHits{200};      //-maxTransHits=N Maximum number of hits for a translated query that are sent from the
                              // server"
  int minMatch{gfMinMatch};   //-minMatch=N   Number of n-mer matches that trigger detailed alignment Default
                              // is 2 for nucleotides, 3 for proteins
  int repMatch{};             //-repMatch=N    Number of occurrences of a tile (n-mer) that triggers repeat masking
                              // the  tile
  bool seqLog{false};         //-seqLog   Include sequences in log file (not logged with  -syslog)
  bool ipLog{false};          //-ipLog    Include user's IP in log file (not logged with -syslog)
  bool debugLog{false};       //-debugLog   Include debugging info in log file

  int tileSize{gfTileSize};     //-tileSize=N   Size of n-mers to index.  Default is 11 for nucleotides, 4 for
  int stepSize{tileSize};       //-stepSize=N     Spacing between tiles. Default is tileSize
                                // proteins (or translated nucleotides)
  bool trans{false};            //-trans      Translate database to protein in 6 frames.  Note: it is best to run
                                // this on RepeatMasked data in this case
  bool syslog{false};           //-syslog     Log to syslog.
  std::string perSeqMax{};      //-perSeqMax=file File contains one seq filename (possibly with ':seq' suffix) per line
  bool noSimpRepMask{false};    //-noSimpRepMask  Suppresses simple repeat masking
  std::string indexFile{};      //-indexFile   Index file create by `gfServer index'. Saving index  can
                                // speed up gfServer startup by two orders of magnitude.
                                // The parameters must  exactly match the parameters when
                                // the file is written or bad things  will happen
  int timeout{90};              //-timeout=N  Timeout in seconds
  std::string genome{};         // no need to get
  std::string genomeDataDir{};  // no need to get

  int threads{2};
  bool allowOneMismatch{false};

  gfServerOption() = default;
  self &build();

  std::string to_string() const;

  gfServerOption &withCanStop(bool canStop_);
  gfServerOption &withLogFacility(std::string logFacility_);
  gfServerOption &withLog(std::string log_);
  gfServerOption &withMask(bool mask_);
  gfServerOption &withMaxAaSize(int maxAaSize_);
  gfServerOption &withMaxDnaHits(int maxDnaHits_);
  gfServerOption &withMaxGap(int maxGap_);
  gfServerOption &withMaxNtSize(int maxNtSize_);
  gfServerOption &withMaxTransHits(int maxTransHits_);
  gfServerOption &withMinMatch(int minMatch_);
  gfServerOption &withRepMatch(int repMatch_);
  gfServerOption &withSeqLog(bool seqLog_);
  gfServerOption &withIpLog(bool ipLog_);
  gfServerOption &withDebugLog(bool debugLog_);
  gfServerOption &withTileSize(int tileSize_);
  gfServerOption &withStepSize(int stepSize_);
  gfServerOption &withTrans(bool trans_);
  gfServerOption &withSyslog(bool syslog_);
  gfServerOption &withPerSeqMax(std::string perSeqMax_);
  gfServerOption &withNoSimpRepMask(bool noSimpRepMask_);
  gfServerOption &withIndexFile(std::string indexFile_);
  gfServerOption &withTimeout(int timeout_);
  gfServerOption &withThreads(int threads_);

  friend std::ostream &operator<<(std::ostream &os, const self &option);
};

void gfServer(gfServerOption &options);
bool boolean2bool(boolean b);

boolean bool2boolean(bool b);

void setSendOk(boolean &sendOk);
void errSendString(int sd, char *s, boolean &sendOk);
void errSendLongString(int sd, char *s, boolean &sendOk);
void logGenoFind(struct genoFind *gf);
void logGenoFindIndex(struct genoFindIndex *gfIdx);

// void genoFindDirect(char *probeName, int fileCount, char *seqFiles[]);
void genoFindDirect(std::string &probeName, int fileCount, std::vector<std::string> &seqFiles,
                    gfServerOption const &options);

// void genoPcrDirect(char *fPrimer, char *rPrimer, int fileCount, char *seqFiles[]);
void genoPcrDirect(std::string &fPrimer, std::string &rPrimer, int fileCount, std::vector<std::string> &seqFiles,
                   gfServerOption const &options);

/* Load up index and hang out in RAM. */
// void startServer(char *hostName, char *portName, int fileCount, char *seqFiles[]);
void startServer(std::string &hostName, std::string &portName, int fileCount, std::vector<std::string> &seqFiles,
                 gfServerOption &options, UsageStats &stats, Signal &signal);

// void stopServer(char *hostName, char *portName);
void stopServer(std::string &hostName, std::string &portName);

// void queryServer(char *type, char *hostName, char *portName, char *faName, boolean complex, boolean isProt);
void queryServer(std::string &type, std::string &hostName, std::string &portName, std::string &faName, bool complex,
                 bool isProt);

/* Do a PCR query to server daemon. */
// void pcrServer(char *hostName, char *portName, char *fPrimer, char *rPrimer, int maxSize);
void pcrServer(std::string &hostName, std::string &portName, std::string &fPrimer, std::string &rPrimer, int maxSize);

// int statusServer(char *hostName, char *portName);
int statusServer(std::string &hostName, std::string &portName, gfServerOption &options);

// void getFileList(char *hostName, char *portName);
void getFileList(std::string &hostName, std::string &portName);

// build pre-computed index for seqFiles and write to
// void buildIndex(char *gfxFile, int fileCount, char *seqFiles[]);
void buildIndex(std::string &gfxFile, int fileCount, std::vector<std::string> seqFiles, gfServerOption const &options);

int getPortIx(char *portName);

/* Handle a query for DNA/DNA match. */
// void dnaQuery(struct genoFind *gf, struct dnaSeq *seq, int connectionHandle, struct hash *perSeqMaxHash);
void dnaQuery(struct genoFind *gf, struct dnaSeq *seq, int connectionHandle, struct hash *perSeqMaxHash,
              gfServerOption const &options, UsageStats &stats, boolean &sendOk);

// void transQuery(struct genoFind *transGf[2][3], aaSeq *seq, int connectionHandle);
void transQuery(struct genoFind *transGf[2][3], aaSeq *seq, int connectionHandle, gfServerOption const &options,
                UsageStats &stats, boolean &sendOk);

// void transTransQuery(struct genoFind *transGf[2][3], struct dnaSeq *seq, int connectionHandle);
void transTransQuery(struct genoFind *transGf[2][3], struct dnaSeq *seq, int connectionHandle,
                     gfServerOption const &options, UsageStats &stats, boolean &sendOk);

void errorSafeQuery(boolean doTrans, boolean queryIsProt, struct dnaSeq *seq, struct genoFindIndex *gfIdx,
                    int connectionHandle, char *buf, struct hash *perSeqMaxHash, gfServerOption const &options,
                    UsageStats &stats, boolean &sendOk);

void checkIndexFileName(char *gfxFile, char *seqFile, gfServerOption const &options);

struct genoFindIndex *loadGfIndex(char *gfIdxFile, boolean isTrans, gfServerOption &options);
void dynSessionInit(struct dynSession *dynSession, char *rootDir, char *genome, char *genomeDataDir, boolean isTrans,
                    gfServerOption &options);
int dynNextCommand(char *rootDir, struct dynSession *dynSession, char **args, gfServerOption &options);

/* NOTE: will change options' value <05-03-23, Yangyang Li yangyang.li@northwestern.edu> */
bool dynamicServerCommand(char *rootDir, struct dynSession *dynSession, gfServerOption &options, UsageStats &stats,
                          boolean &sendOk);
void dynamicServer(std::string &rootDir, gfServerOption &options, UsageStats &stats, boolean &sendOk);

struct dnaSeq *dynReadQuerySeq(int qSize, boolean isTrans, boolean queryIsProt, gfServerOption const &options);

void dynamicServerQuery(struct dynSession *dynSession, int numArgs, char **args, gfServerOption const &options,
                        UsageStats &stats, boolean &sendOk);

struct hash *maybePerSeqMax(int fileCount, char *seqFiles[], gfServerOption &options);
boolean badPcrPrimerSeq(char *s);

void setSocketTimeout(int sockfd, int delayInSeconds);
void hashZeroVals(struct hash *hash);
void errorSafePcr(struct genoFind *gf, char *fPrimer, char *rPrimer, int maxDistance, int connectionHandle,
                  boolean &sendOk);

genoFindIndex *pybuildIndex4Server(std::string &hostName, std::string &portName, int fileCount, char *seqFiles[],
                                   hash *perSeqMaxHash, gfServerOption &option);

std::string pystatusServer(std::string &hostName, std::string &portName, gfServerOption &options);
std::string pygetFileList(std::string &hostName, std::string &portName);
std::string pyqueryServer(std::string &type, std::string &hostName, std::string &portName, std::string &faName,
                          bool complex, bool isProt);

void test_stdout();
void test_add(int &a);
void test_stat(UsageStats &stats);

}  // namespace cppbinding
#endif
