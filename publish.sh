#!/bin/bash

outdir=ep1-thiago-wilson

if [ ! -d $outdir ]; then
  mkdir $outdir
fi

contents="src/ Makefile objs.makefile deps.makefile LEIAME www/"
cp -r $contents $outdir

tar -caf $outdir.tar.gz $outdir

rm -rf $outdir

