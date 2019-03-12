
out = File.open("q2.dat","w")
`ls q2*.dat`.split(/\n/).each do |file|
  File.open(file) do |f|
    while line = f.gets
      a = line.split(/\s/)
      out.puts "#{a[2]} #{a[1]}"
    end
  end
end

out = File.open("sc.dat","w")
`ls sc*.dat`.split(/\n/).each do |file|
  File.open(file) do |f|
    while line = f.gets
      a = line.split(/\s/)
      out.puts "#{a[2]} #{a[1]}"
    end
  end
end
