using Microsoft.EntityFrameworkCore.Migrations;

#nullable disable

namespace AgOpenNtripCaster.Server.Data.Migrations
{
    /// <inheritdoc />
    public partial class AddSourcetableFieldsToMountPoint : Migration
    {
        /// <inheritdoc />
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.AddColumn<string>(
                name: "FormatDetails",
                table: "MountPoints",
                type: "text",
                nullable: true);

            migrationBuilder.AddColumn<string>(
                name: "Identifier",
                table: "MountPoints",
                type: "text",
                nullable: true);
        }

        /// <inheritdoc />
        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropColumn(
                name: "FormatDetails",
                table: "MountPoints");

            migrationBuilder.DropColumn(
                name: "Identifier",
                table: "MountPoints");
        }
    }
}
