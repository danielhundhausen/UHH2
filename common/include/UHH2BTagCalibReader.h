#include <boost/optional.hpp>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <UHH2/common/include/exprtk.hpp> // toolkit for compiling strings to cpp-valid formulas during runtime (similar to TFormula, but more powerful and efficient)

#define F_MAX std::numeric_limits<float>::max()
#define F_MIN std::numeric_limits<float>::min()
#define CENTRAL_CSV_STRING "central"

namespace uhh2 { namespace BTagCalib {

enum class OperatingPoint {
  SHAPE,
  LOOSE,
  MEDIUM,
  TIGHT,
  UNDEFINED,
};

typedef struct {
  std::string csv_string;
  int index;
} OperatingPointInfo;

const std::map<OperatingPoint, OperatingPointInfo> kOperatingPoints = {
  { OperatingPoint::SHAPE, { .csv_string = "shape", .index=-1 } },
  { OperatingPoint::LOOSE, { .csv_string = "L", .index=0 } },
  { OperatingPoint::MEDIUM, { .csv_string = "M", .index=1 } },
  { OperatingPoint::TIGHT, { .csv_string = "T", .index=2 } },
};

enum class JetFlavor {
  FLAV_B,
  FLAV_C,
  FLAV_UDSG,
  UNDEFINED,
};

typedef struct {
  std::string csv_string;
} JetFlavorInfo;

const std::map<JetFlavor, JetFlavorInfo> kJetFlavors = {
  { JetFlavor::FLAV_B, { .csv_string = "5" } },
  { JetFlavor::FLAV_C, { .csv_string = "4" } },
  { JetFlavor::FLAV_UDSG, { .csv_string = "0" } },
};

typedef exprtk::symbol_table<float> symbol_table_t;
typedef exprtk::expression<float> expression_t;
typedef exprtk::parser<float> parser_t;

class Reader {

public:

  Reader(
    const std::string & csvFilePath,
    const OperatingPoint op,
    const std::string & measurementType,
    const boost::optional<std::set<std::string> &> sysTypes = boost::none,
    const boost::optional<bool> absEta = boost::none,
    const boost::optional<bool> verbose = boost::none,
    const boost::optional<float> defaultScaleFactor = boost::none,
    const boost::optional<bool> failIfNoEntry = boost::none
  );

  float Evaluate(
    const std::string & sysType,
    const JetFlavor jf,
    float eta,
    float pt,
    float discr = 0.f
  );

private:

  symbol_table_t fSymbolTable;
  parser_t fParser;
  float fVariable;

  typedef struct {
    float etaMin = F_MIN;
    float etaMax = F_MAX;
    float ptMin = F_MIN;
    float ptMax = F_MAX;
    float discrMin = F_MIN;
    float discrMax = F_MAX;
    std::shared_ptr<expression_t> func;
  } CalibEntry;

  void ReadCSV();

  const std::string fCSVFilePath;
  const OperatingPoint fOperatingPoint;
  const std::string fMeasurementType;
  const bool bAbsEta;
  const bool bVerbose;
  const float kDefaultScaleFactor;
  const bool bFailIfNoEntry;
  const bool bShape;

  std::set<std::string> fSysTypes = { CENTRAL_CSV_STRING };

  std::map<std::string, std::map<JetFlavor, std::vector<CalibEntry>>> fCalibEntries;
};

}}
