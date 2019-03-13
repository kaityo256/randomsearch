
out = File.open("q2.dat", "w")
arr = []
`ls q2*.dat`.split(/\n/).each do |file|
  File.open(file) do |f|
    while line = f.gets
      a = line.split(/\s/)
      arr.push [a[2].to_i, a[1].to_i]
    end
  end
end

arr.sort! do |a, b|
  if a[0] == b[0]
    a[1] <=> b[1]
  else
    a[0] <=> b[0]
  end
end

arr.each do |v|
  out.puts "#{v[0]} #{v[1]}"
end

arr = []

out = File.open("sc.dat", "w")
`ls sc*.dat`.split(/\n/).each do |file|
  File.open(file) do |f|
    while line = f.gets
      a = line.split(/\s/)
      arr.push [a[2].to_i, a[1].to_i]
    end
  end
end
arr.sort! do |a, b|
  if a[0] == b[0]
    a[1] <=> b[1]
  else
    a[0] <=> b[0]
  end
end

arr.each do |v|
  out.puts "#{v[0]} #{v[1]}"
end

