library(glue)
library(withr)
library(stringi)
library(profvis)


# if we want to simulate really large files, would want to buffer and
# write to file rather than doing it all @ once
sim_fasta = function(seqlens,linewidth = Inf,chars = "ACGT") {
  
  L = length(seqlens)
  seqs = stri_rand_strings(L,seqlens,pattern = glue("[{chars}]"))
  
  if(linewidth < max(seqlens)) {
    seqs = sapply(seq_along(seqs),function(i) {
      breaks = seq.int(1L,seqlens[i],by = linewidth)
      stri_join(stri_sub(seqs[i],breaks,breaks + linewidth - 1),collapse = "\n")
    })
  }
  
  res = vector(mode = 'character',length = 2*L)
  res[seq.int(1L,2L*L,by=2L)] = paste0('>Seq_',1:L)
  res[seq.int(2L,2L*L,by=2L)] = seqs
  res
}

sim_fastq = function(seqlens,chars = "ACGT") {
  
  L = length(seqlens)
  qual_chars = "[[!\"#$%\\&'\\(\\)*+,./\\:;<=>?@~\\^_`][a-zA-Z0-9][\\{\\}\\|\\-\\[\\]\\\\]]"
  # stri_rand_strings(10,10,qual_chars)
  seqs = stri_rand_strings(L,seqlens,pattern = glue("[{chars}]"))
  quals = stri_join("+\n",stri_rand_strings(L,seqlens,pattern = qual_chars))
  res = vector(mode = 'character',length = 3*L)
  res[seq.int(1L,3L*L,by=3L)] = paste0('@Seq_',1:L)
  res[seq.int(2L,3L*L,by=3L)] = seqs
  res[seq.int(3L,3L*L,by=3L)] = quals
  res
}

n_seqs = 2000
avgseqlen = 100000
profvis(
  {
    myf = sim_fasta(rpois(n_seqs,avgseqlen),100)
  }
)

myfa = sim_fasta(rpois(n_seqs,avgseqlen),100)
writeLines(myfa,con = 'C:/Users/skm/Dropbox/bv/test_fasta.fasta')

myfq = sim_fastq(rpois(n_seqs,avgseqlen))
writeLines(myfq,con = 'C:/Users/skm/Dropbox/bv/test_fastq.fastq')
