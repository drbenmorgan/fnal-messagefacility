use strict;

package CetSkelPlugins::ELdestination;

# Not currently useful (2015/04/17), since we provide our own version of
# the only function it provides for us.
#  use CetSkel::BasicPluginCommon;

use vars qw(@ISA);

eval "use CetSkel::messagefacility::PluginVersionInfo";
unless ($@) {
  push @ISA, "CetSkel::messagefacility::PluginVersionInfo";
}

sub new {
  my $class = shift;
  my $self = { };
  return bless \$self, $class;
}

sub type { return "eldestination"; }

sub source { return __FILE__; }

sub baseClasses {
  return
    [
     { header => 'messagefacility/MessageLogger/ELdestination.h',
       class => "mf::service::ELdestination",
       protection => "public" # Default.
     }
    ];
}

sub constructors {
  return [ {
            explicit => 1,
            args => [ "fhicl::ParameterSet const & p" ],
            initializers => [ "ELdestination(p)" ]
           } ];
}

sub defineMacro {
  my ($self, $qual_name) = @_;
  return <<EOF;
extern "C" {
  auto makePlugin(std::string const & psetName,
                  fhicl::ParameterSet const & pset)
  {
    return std::make_unique<${qual_name}>( /* args as necessary. */ );
  }
}
DEFINE_BASIC_PLUGINTYPE_FUNC(mf::service::ELdestination)
EOF
}

# No sub implHeaders necessary.

# No sub macrosInclude necessary.

sub optionalEntries {
  return
    {
     changeFileELstring => 'void changeFile (mf::ELstring const & filename) override',
     changeFileOS => 'void changeFile (std::ostream & os) override',
     clearSummary => 'void clearSummary() override',
     excludeModule => 'void excludeModule(mf::ELstring const & moduleName) override',
     fillPrefix => 'void fillPrefix(std::ostringstream& oss, const mf::ErrorObj& msg) override',
     fillSuffix => 'void fillSuffix(std::ostringstream& oss, const mf::ErrorObj& msg) override',
     fillUsrMsg => 'void fillUsrMsg(std::ostringstream& oss, const mf::ErrorObj& msg) override',
     filterModule => 'void filterModule(mf::ELstring const & moduleName) override',
     finish => 'void finish() override',
     flush => 'void flush() override',
     getLineLength => 'int getLineLength() const override',
     getNewline => 'mf::ELstring getNewline() const override',
     ignoreModule => 'void ignoreModule(mf::ELstring const & moduleName) override',
     log => 'void log(mf::ErrorObj & msg) override',
     noTerminationSummary => 'void noTerminationSummary() override',
     respondToModule => 'void respondToModule(mf::ELstring const & moduleName) override',
     routePayload => 'void routePayload(const std::ostringstream& oss, const mf::ErrorObj& msg) override',
     setLineLength => 'int setLineLength(int len) override',
     setTableLimit => 'void setTableLimit(int n) override',
     statisticsMap => 'std::map<mf::ELextendedID, mf::StatsCount> statisticsMap() const override',
     summarization => 'void summarization(mf::ELstring const & title, mf::ELstring const & sumLines) override',
     summary => 'void summary() override',
     summaryElstring => 'void summary(mf::ELstring & s, mf::ELstring const & title=) override',
     summaryForJobReport => 'void summaryForJobReport(std::map<std::string, double> & sm) override',
     summaryOS => 'void summary(std::ostream & os, mf::ELstring const & title=) override',
     switchChannel => 'bool switchChannel(mf::ELstring const & channelName) override',
     thisShouldBeIgnored => 'bool thisShouldBeIgnored(mf:ELstring const & s) const override',
     wipe => 'void wipe() override',
     zero => 'void zero() override'
    };
}

sub pluginSuffix {
  return "_mfplugin";
}

# No sub requiredEntries necessary.

1;
