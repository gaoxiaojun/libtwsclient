require 'formula'

class Libtwsclient < Formula
  homepage 'http://www.dailypips.org'
  url 'https://github.com/gaoxiaojun/libtwsclient/archive/0.0.1.tar.gz'
  sha256 '3fb73364a5a30efe615046d07e6db9d09fd2b41c763c5f7d3bfb121cd5c5ac5a'

  # Configure uses cmake internally
  depends_on 'cmake' => :build

  def install
    ENV.deparallelize

    system "cmake", ".", *std_cmake_args
    system "make install"
  end

end
