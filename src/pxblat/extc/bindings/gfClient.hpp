#ifndef GFCLIENT_HPP
#define GFCLIENT_HPP

#include <ostream>
#include <string>

#include "aliType.h"
#include "common.h"
#include "errCatch.h"
#include "fa.h"
#include "fuzzyFind.h"
#include "genoFind.h"
#include "linefile.h"
#include "options.h"
#include "portable.h"
#include "psl.h"

namespace cppbinding {
// "   -t=type       Database type. Type is one of:\n"
// "                   dna - DNA sequence\n"
// "                   prot - protein sequence\n"
// "                   dnax - DNA sequence translated in six frames to protein\n"
// "                 The default is dna.\n"
// "   -q=type       Query type. Type is one of:\n"
// "                   dna - DNA sequence\n"
// "                   rna - RNA sequence\n"
// "                   prot - protein sequence\n"
// "                   dnax - DNA sequence translated in six frames to protein\n"
// "                   rnax - DNA sequence translated in three frames to protein\n"
// "   -prot         Synonymous with -t=prot -q=prot.\n"
// "   -dots=N       Output a dot every N query sequences.\n"
// "   -nohead       Suppresses 5-line psl header.\n"
// "   -minScore=N   Sets minimum score.  This is twice the matches minus the \n"
// "                 mismatches minus some sort of gap penalty.  Default is 30.\n"
// "   -minIdentity=N   Sets minimum sequence identity (in percent).  Default is\n"
// "                 90 for nucleotide searches, 25 for protein or translated\n"
// "                 protein searches.\n"
// "   -out=type     Controls output file format.  Type is one of:\n"
// "                   psl - Default.  Tab-separated format without actual sequence\n"
// "                   pslx - Tab-separated format with sequence\n"
// "                   axt - blastz-associated axt format\n"
// "                   maf - multiz-associated maf format\n"
// "                   sim4 - similar to sim4 format\n"
// "                   wublast - similar to wublast format\n"
// "                   blast - similar to NCBI blast format\n"
// "                   blast8- NCBI blast tabular format\n"
// "                   blast9 - NCBI blast tabular format with comments\n"
// "   -maxIntron=N   Sets maximum intron size. Default is %d.\n"
// "   -genome=name  When using a dynamic gfServer, The genome name is used to \n"
// "                 find the data files relative to the dynamic gfServer root, named \n"
// "                 in the form $genome.2bit, $genome.untrans.gfidx, and $genome.trans.gfidx\n"
// "   -genomeDataDir=path\n"
// "                 When using a dynamic gfServer, this is the dynamic gfServer root directory\n"
// "                 that contained the genome data files.  Defaults to being the root directory.\n"
// "                \n",

// ariables that can be overridden by command line. */
// int dots = 0;
// int minScore = 30;
// double minIdentity = 90;
// char *outputFormat = "psl";
// char *qType = "dna";
// char *tType = "dna";
// char *genome = NULL;
// char *genomeDataDir = NULL;
// boolean isDynamic = FALSE;
// long enterMainTime = 0;

struct gfClientOption {
  std::string hostName{};
  std::string portName{};

  std::string tType{"dna"};
  std::string qType{"dna"};
  int dots{0};
  bool nohead{false};
  int minScore{30};
  double minIdentity{90.0};
  std::string outputFormat{"psl"};
  long maxIntron{ffIntronMaxDefault};
  std::string genome{};
  std::string genomeDataDir{};
  bool isDynamic{false};

  std::string tSeqDir{};
  std::string inName{};
  std::string outName{};

  std::string inSeq{};

  gfClientOption() = default;

  gfClientOption &build();

  gfClientOption &withHost(const std::string &hostName_);
  gfClientOption &withPort(const std::string &portName_);

  gfClientOption &withTType(const std::string &tType_);
  gfClientOption &withQType(const std::string &qType_);
  gfClientOption &withDots(int dots_);
  gfClientOption &withNohead(bool nohead_);
  gfClientOption &withMinScore(int minScore_);
  gfClientOption &withMinIdentity(double minIdentity_);
  gfClientOption &withOutputFormat(const std::string &outputFormat_);
  gfClientOption &withMaxIntron(long maxIntron_);
  gfClientOption &withGenome(const std::string &genome_);
  gfClientOption &withGenomeDataDir(const std::string &genomeDataDir_);
  gfClientOption &withIsDynamic(bool isDynamic_);
  gfClientOption &withTSeqDir(const std::string &tSeqDir_);
  gfClientOption &withInName(const std::string &inName_);
  gfClientOption &withOutName(const std::string &outName_);
  gfClientOption &withInseq(const std::string &inseq_);

  std::string to_string() const;
  friend std::ostream &operator<<(std::ostream &os, const gfClientOption &option);
};

std::string pygfClient(gfClientOption &option);
std::string read_inmem_file(FILE *file);
}  // namespace cppbinding

#endif
