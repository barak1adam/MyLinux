# import build definition file
. build/scripts/config

# Set the prefix of the build path
build_path=build/dist/build-${TOOLCOMBO}

kwdir=%teamcity.agent.work.dir%/.klocwork
tbldir=%teamcity.build.checkoutDir%/kw_utils/tables
rm -rf $tbldir/.kwlp
rm -rf $tbldir/.kwps
mkdir -p $tbldir

echo "--------===----------"
echo "%teamcity.build.branch%"
echo "%teamcity.build.vcs.branch.Os_MsysFamily_NgMosRepo11%"
echo "build_path = ${build_path}"

if [ "%teamcity.build.branch%" = "<default>" ] || [ "%teamcity.build.branch%" = "master" ];
then
  echo %system.teamcity.build.changedFiles.file%
  if ! [ -s %system.teamcity.build.changedFiles.file% ];
  then
    echo "no file change, exit step"
    exit
  fi;
  cp %system.teamcity.build.changedFiles.file% ./tmp.txt
else
  cd %teamcity.build.checkoutDir%/router_code
  git fetch --progress origin +refs/heads/master:refs/heads/master
  git diff --name-only %teamcity.build.branch% `git merge-base %teamcity.build.branch% master` > %teamcity.build.checkoutDir%/tmp.txt
  cd -
fi;

sed -i "s/:.\+$//g" ./tmp.txt
# sed -i "s/^/\.\//g" ./tmp.txt
sed -i "s#^#${build_path}/#" ./tmp.txt
sed -i "s#router_code/##" ./tmp.txt
sed -i "s#oam_code/##" ./tmp.txt
cat ./tmp.txt | grep -i "\.c\(pp\)\?$" > ./files.list
if ! [ -s ./files.list ];
then
  echo "no c/cpp file change, exit step"
  exit
fi;

./kw_utils/parse_local_results.py -b $kwdir/buildspecs/OS/%system.teamcity.buildType.id%/RUNNER_45_CPP.out -d $tbldir/.kwlp -s $tbldir/.kwps -f ./files.list -t %teamcity.build.triggeredBy% -p OS -R ${BUILD_DIR}

echo "--------===----------"
