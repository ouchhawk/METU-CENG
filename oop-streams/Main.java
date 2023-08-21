import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;
import java.util.stream.Stream;


public class Main {

  public static void main(String[] args) throws IOException {

	PartsStore ps = new PartsStore();
	//ps.FindPartsWithBrand("Memory","Kingston");
	ps.FindFastestCPU();
	ps.FindCheapestMemory(16);
	ps.TotalPrice("Hard Drive", "Crucial","MX500");
	ps.UpdateStock();
	ps.FindFastestCPU();
	ps.FindCheapestMemory(16);
	ps.TotalPrice("Hard Drive", "Crucial","MX500");
	

	}

}