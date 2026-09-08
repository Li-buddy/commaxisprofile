# This file checks if debug and release is available in given path
# e.g. ./analyse_buildresult.ps1  ./../../public/bin/win-msvc-x64
#
# usage as power shell call from jenkinsfile:
#
#  powershell(script: 'private/make/analyse_buildresult.ps1 public/bin/win-msvc-x64',
#                     label: 'analyse buildresult')
#



param (
   [parameter(Mandatory=$true,HelpMessage="path to check")]$buildpath
)

$debugFound = 0
$releaseFound = 0
Get-ChildItem -Path $buildpath |  
ForEach-Object{
  $filepath = $buildpath +"/"+ $_
  Write-Output $filepath
  
  if(Test-Path -Path $filepath -PathType Container)
  {
    if( $filepath -match "debug" -or $filepath -match "Debug")
    {
      $filesDebug = Get-ChildItem -Path $filepath -Filter *.dll
      Write-Output ("Is debug folder with {0} dll found" -f ($filesDebug.Count ))
      if($filesDebug.Count -gt 0)
      {
        $debugFound = 1
      }
    }
    if( $filepath -match "release" -or $filepath -match "Release")
    {
      $filesRelease = Get-ChildItem -Path $filepath -Filter *.dll
      Write-Output ("Is release folder with {0} dll found" -f ($filesRelease.Count ))
      if($filesRelease.Count -gt 0)
      {
        $releaseFound = 1
      }
    }    
  }
}
if( $debugFound -eq 0)
{
  Write-Output "Debug folder is missing"
  exit 1
}
if( $releaseFound -eq 0)
{
  Write-Output "Release folder is missing"
  exit 1
}

Write-Output "All necessary folders found"
exit 0
