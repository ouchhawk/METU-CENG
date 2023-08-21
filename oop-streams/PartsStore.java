import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.io.UncheckedIOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;
import java.util.stream.Collectors;
import java.io.FileWriter;

public class PartsStore {
  private String path;

  public String getPath(){
    return this.path;
  }

  public PartsStore() {
    path = "pcparts.csv";
  }

  /** Prints all the parts of type with brand. If the type is null it returns all items with brand regardless of their types. */
  public void FindPartsWithBrand(String type, String brand) throws IOException {

    List<List<String>> matchedItemsList = Files.lines(Paths.get(this.path))
                                              .map(line -> Arrays.asList(line.split(",")))
                                              .filter(line -> line.get(0).equals(type))
                                              .filter(line -> line.get(1).equals(brand))
                                              .distinct()
                                              .collect(Collectors.toList());
    
    for(List<String> item : matchedItemsList){
      System.out.println(String.join(",",item));
    }
  }

  /** Prints the total price of the parts with type, brand and model. In case of one or many arguments being null, total price of all parts with the not null
   * arguments are returned. For example, TotalPrice(null, "Asus", null) will return total price of all parts with the brand "Asus". */
  public void TotalPrice(String type, String brand, String model) throws IOException {
    Double sum=0.0;
    if(type==null && brand!=null && model!=null){
      sum = Files.lines(Paths.get(this.path))
                  .map(line -> Arrays.asList(line.split(",")))
                  .filter(line -> line.get(1).equals(brand))
                  .filter(line -> line.get(2).equals(model))
                  .map(line -> Double.valueOf(line.get(line.size()-1).substring(0, line.get(line.size()-1).length()-4)))
                  .mapToDouble(Double::doubleValue)
                  .sum();
    } 
    else if(type!=null && brand==null && model!=null){
      sum = Files.lines(Paths.get(this.path))
                  .map(line -> Arrays.asList(line.split(",")))
                  .filter(line -> line.get(0).equals(type))
                  .filter(line -> line.get(2).equals(model))
                  .map(line -> Double.valueOf(line.get(line.size()-1).substring(0, line.get(line.size()-1).length()-4)))
                  .mapToDouble(Double::doubleValue)
                  .sum();
    }
    else if(type!=null && brand!=null && model==null){
      sum = Files.lines(Paths.get(this.path))
                  .map(line -> Arrays.asList(line.split(",")))
                  .filter(line -> line.get(0).equals(type))
                  .filter(line -> line.get(1).equals(brand))
                  .map(line -> Double.valueOf(line.get(line.size()-1).substring(0, line.get(line.size()-1).length()-4)))
                  .mapToDouble(Double::doubleValue)
                  .sum();
    }
    else if(type==null && brand!=null && model==null){
      sum = Files.lines(Paths.get(this.path))
                  .map(line -> Arrays.asList(line.split(",")))
                  .filter(line -> line.get(1).equals(brand))
                  .map(line -> Double.valueOf(line.get(line.size()-1).substring(0, line.get(line.size()-1).length()-4)))
                  .mapToDouble(Double::doubleValue)
                  .sum();
    }
    else if(type!=null && brand==null && model==null){
      sum = Files.lines(Paths.get(this.path))
                  .map(line -> Arrays.asList(line.split(",")))
                  .filter(line -> line.get(0).equals(type))
                  .map(line -> Double.valueOf(line.get(line.size()-1).substring(0, line.get(line.size()-1).length()-4)))
                  .mapToDouble(Double::doubleValue)
                  .sum();
    }
    else if(type==null && brand==null && model!=null){
      sum = Files.lines(Paths.get(this.path))
                  .map(line -> Arrays.asList(line.split(",")))
                  .filter(line -> line.get(2).equals(model))
                  .map(line -> Double.valueOf(line.get(line.size()-1).substring(0, line.get(line.size()-1).length()-4)))
                  .mapToDouble(Double::doubleValue)
                  .sum();
    }
    else if(type==null && brand==null && model==null){
      sum = Files.lines(Paths.get(this.path))
                  .map(line -> Arrays.asList(line.split(",")))
                  .map(line -> Double.valueOf(line.get(line.size()-1).substring(0, line.get(line.size()-1).length()-4)))
                  .mapToDouble(Double::doubleValue)
                  .sum();
    }
    else if(type!=null && brand!=null && model!=null){
      sum = Files.lines(Paths.get(this.path))
                  .map(line -> Arrays.asList(line.split(",")))
                  .filter(line -> line.get(0).equals(type))
                  .filter(line -> line.get(1).equals(brand))
                  .filter(line -> line.get(2).equals(model))
                  .map(line -> Double.valueOf(line.get(line.size()-1).substring(0, line.get(line.size()-1).length()-4)))
                  .mapToDouble(Double::doubleValue)
                  .sum();
    }

    System.out.println(sum + " USD");
  }

  /** Discards the parts that are not in the stock right now (Parts with price value set to 0 USD) and prints how many items are discarded. 
   *  After this method is called, other methods should work over the updated stock, so that they would never return an item with price 0 USD. */
  public void UpdateStock() throws IOException {

    long itemCountBefore = Files.lines(Paths.get(this.path)).count();
    
    List<List<String>> matchedItemsList = Files.lines(Paths.get(this.path))
                                              .map(line -> Arrays.asList(line.split(",")))
                                              .filter(line -> (Double.valueOf(line.get(line.size()-1).substring(0, line.get(line.size()-1).length()-4))) > 0.00)
                                              .distinct()
                                              .collect(Collectors.toList());

    /** Overwrite "pcparts.csv" */
    FileWriter file = new FileWriter(this.path);
    for (List<String> item : matchedItemsList) {
      file.append(String.join(",", item) + "\n");
    }
    file.flush();
    file.close();

    long itemCountAfter = Files.lines(Paths.get(this.path)).count();

    System.out.println((itemCountBefore-itemCountAfter) + " items removed.");
    
  }

  /** Prints the details of the cheapest Memory part with equal or larger capacity than capacity. */
  public void FindCheapestMemory(int capacity) throws IOException {
    List<List<String>> cheapestMemoryAsList = Files.lines(Paths.get(this.path))
                                                  .map(line -> Arrays.asList(line.split(",")))
                                                  .filter(line -> line.get(0).equals("Memory"))
                                                  .filter(line -> Integer.valueOf(line.get(4).substring(0, line.get(4).length()-2))>=capacity)
                                                  .distinct()
                                                  .sorted((l1,l2) -> Double.valueOf(l1.get(l1.size()-1).substring(0, l1.get(l1.size()-1).length()-4)).compareTo(Double.valueOf(l2.get(l2.size()-1).substring(0, l2.get(l2.size()-1).length()-4))))
                                                  .collect(Collectors.toList());
                    
    System.out.println(String.join(",",cheapestMemoryAsList.get(0)));
  }

  /** Prints the details of the CPU with the highest value of (core count * clock speed) */
  public void FindFastestCPU() throws IOException {

    List<List<String>> fastestCPUAsList = Files.lines(Paths.get(this.path))
                                              .map(line -> Arrays.asList(line.split(",")))
                                              .filter(line -> line.get(0).equals("CPU"))
                                              .distinct()
                                              /** 1. Convert truncated core number and frequency fields into Double objects
                                               *  2. Multiply their values in primitive form then cast to a Double object again to be able to use .compareTo method */ 
                                              .sorted((l2,l1) -> Double.valueOf(Double.valueOf(l1.get(3)).doubleValue() * Double.valueOf(l1.get(4).substring(0, l1.get(4).length()-3)).doubleValue()) 
                                                  .compareTo (Double.valueOf(Double.valueOf(l2.get(3)).doubleValue() * Double.valueOf(l2.get(4).substring(0, l2.get(4).length()-3)).doubleValue())))
                                              .collect(Collectors.toList());
                    
    System.out.println(String.join(",",fastestCPUAsList.get(0)));
  }
}
